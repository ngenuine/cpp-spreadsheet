#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <deque>
#include <unordered_set>

using namespace std::literals;

Cell::Cell(SheetInterface& sheet)
    : sheet_(sheet)
{
}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    if (text.size() > 1 && *text.begin() == FORMULA_SIGN) {
        // ParseFormula бросит FormulaException при синтаксически некорректной формуле
        auto formula = ParseFormula(std::string(text.begin() + 1, text.end()));

        auto refs = formula->GetReferencedCells();
        // проверяем что не принесли циклов в таблицу
        if (CheckCycles(refs)) {
            std::string as_text = formula->GetExpression();
            throw CircularDependencyException("Have circular dependicies: "s + as_text);
        }

        // все ячейки из фурмулы надо создать в таблице
        for (const auto& pos : refs) {
            if (!sheet_.GetCell(pos)) {    // если ячейка еще не существует
                sheet_.SetCell(pos, ""s);  // создаем пустую
            }
        }

        Clear();
        formula_ = std::move(formula);
    } else {
        Clear();
        text_ = std::move(text);
    }
}

void Cell::Clear() {
    text_ = ""s;
    formula_.release();
}

Cell::Value Cell::GetValue() const {
    if (cashed_value_.has_value()) {
        return cashed_value_.value();
    }

    if (IsEmpty()) {
        cashed_value_ = VALUE_IF_EMPTY_CELL;
        return VALUE_IF_EMPTY_CELL;
    }

    if (text_.size() > 0) { // значит ячейка содержит текст
        if (*text_.begin() == ESCAPE_SIGN) {
            std::string without_escape = std::string(text_.begin() + 1, text_.end());
            cashed_value_ = without_escape;
            return without_escape;
        }

        std::string as_text = GetText();
        cashed_value_ = as_text;

        return as_text;
    }

    // Evaluate внутри себя перехватит любые исключения,
    // возникшие в результате вычисления формулы
    auto result = formula_->Evaluate(sheet_);
    if (std::holds_alternative<double>(result)) {
        double as_double = std::get<double>(result);
        cashed_value_ = as_double;
        return as_double;
    }

    auto as_error = std::get<FormulaError>(result);
    cashed_value_ = as_error;
    return as_error;
}

std::string Cell::GetText() const {
    if (formula_) {
        return FORMULA_SIGN + formula_->GetExpression();
    }

    return text_;
}

std::vector<Position> Cell::GetReferencedCells() const {
    if (formula_) {
        return formula_->GetReferencedCells();
    }

    return {};
}

bool Cell::CheckCycles(const std::vector<Position>& referenced_cells) const {
    auto add_cells = [](const std::vector<Position>& from, std::deque<const CellInterface*>& to, const SheetInterface& sheet) {
        for (const Position& pos : from) {
            const CellInterface* cell = sheet.GetCell(pos);
            if (cell) { // если ячейка, кторая nullptr, не несет циклов по определению
                to.push_back(cell);
            }
        }
    };

    std::deque<const CellInterface*> cells_to_visit; // дек не nullptr указателей на ячейки
    std::unordered_set<const CellInterface*> visited{this};

    add_cells(referenced_cells, cells_to_visit, sheet_);

    while (cells_to_visit.size() > 0) {
        // забираем ячейку из очереди к посещению
        const CellInterface* cell = cells_to_visit.front();
        cells_to_visit.pop_front();

        // если 'cell' была уже посещена, имеет место цикл
        if (visited.count(cell) == 1) {
            return true;
        }
        
        visited.insert(cell);

        // помещаем ячейки, от которых зависит ячейка, от которой зависит 'cell' в деку ячеек к посещению
        add_cells(cell->GetReferencedCells(), cells_to_visit, sheet_);
    }

    // цикла нет
    return false;
}

void Cell::InvalidateCache() {
    // идти по родителям (зависимые ячейки, обратную сторону) и ресетить кэш
    cashed_value_.reset();

    for (CellInterface* icell : parents_) {
        Cell* cell = static_cast<Cell*>(icell);
        if (!cell->IsCacheInvalidated()) {
            cell->InvalidateCache();
        }
    }
}

void Cell::AddThisToChildren() {
    for (const Position& pos : GetReferencedCells()) {
        Cell* cell = static_cast<Cell*>(sheet_.GetCell(pos));
        auto& parents = cell->GetParentSet();
        parents.insert(this);
    }
}

std::unordered_set<Cell*>& Cell::GetParentSet() {
    return parents_;
}

bool Cell::IsEmpty() const {
    return text_.size() == 0 && formula_ == nullptr;
}

bool Cell::IsFormula() const {
    return formula_ != nullptr;
}

void Cell::DeleteThisFromChildren() {
    for (const Position& pos : GetReferencedCells()) {
        Cell* cell = static_cast<Cell*>(sheet_.GetCell(pos));
        auto& parents = cell->GetParentSet();
        parents.erase(this);
    }
}

bool Cell::IsCacheInvalidated() const {
    return !cashed_value_.has_value();
}