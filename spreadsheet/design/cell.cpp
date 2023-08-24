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

// создает ячейку, которая знает о листе, в котором находится, и которая содержит текст или формулу
std::unique_ptr<CellInterface> CreateCell(const std::string& str, const SheetInterface* sheet) {
    std::unique_ptr<CellInterface> cell = std::make_unique<Cell>(*sheet);
    cell->Set(str); // бросит FormulaException при синтаксически некорректной формуле
    return cell;
}

void Cell::Set(std::string text) {
    // Что будет, если до вызова метода Cell::Set() ячейка была пустой?
    // Что будет, если она не была пустой, а содержала формулу с другими зависимостями и была встроена в граф? Как изменение в ней повлияет на другие ячейки?

    if (text.size() > 1 && *text.begin() == FORMULA_SIGN) {
        // ParseFormula бросит FormulaException при синтаксически некорректной формуле
        auto formula = ParseFormula(std::string(text.begin() + 1, text.end()));

        // formula может отдать ячейки, которые в нее входят
        // проверяем что не принесли циклов в таблицу
        if (CheckCycles(formula->GetReferencedCells())) {
            std::string as_text = formula->GetExpression();
            throw CircularDependencyException("Have circular dependicies: "s + as_text);
        }

        // формула в str не несет циклов, пока она не заменила formula_, надо удалить из
        // непосредственных детей информацию, что родительская (эта) ячейка от них зависит
        DeleteThisFromChildren(GetReferencedCells());
        Clear();
        formula_ = std::move(formula);
        AddThisToChildren(GetReferencedCells());        
    } else {
        Clear();
        text_ = std::move(text);

    }

    // расчеты прямо или опосредованно зависимых ячеек (родителей), основанные
    // на данных этой ячейки теперь некорректны, так как теперь у этой ячейки другое значение
    // делаем инвалидацию кэша
    InvalidateCache();
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
            return std::string(text_.begin() + 1, text_.end());
        }

        try {
            // пробуем кастовать текстовое значение в ячейке в число, вдруг получится
            double as_double = std::stod(text_);
            cashed_value_ = as_double;
            return as_double;
        } catch (const std::exception& ex) {

        }

        return GetText();
    }

    // значит ячейка содержит формулу
    // Evaluate внутри себя перехватит любые исключения,
    // возникшие в результате вычисления формулы
    auto result = formula_->Evaluate(sheet_);
    if (std::holds_alternative<double>(result)) {
        // положим в кэш значение
        double as_double = std::get<double>(result);
        cashed_value_ = as_double;
        return as_double;
    }

    auto as_error = std::get<FormulaError>(result);
    cashed_value_ = as_error;
    return as_error;
}

std::string Cell::GetText() const {
    if (formula_) { // значит ячейка содержит формулу
        FORMULA_SIGN + formula_->GetExpression();
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
            to.push_back(sheet.GetCell(pos));
        }
    };

    std::deque<const CellInterface*> cells_to_visit;
    std::unordered_set<const CellInterface*> visited{this};

    add_cells(referenced_cells, cells_to_visit, sheet_);

    while (cells_to_visit.size() > 0) {
        // забираем ячейку из очереди к посещению
        const CellInterface* cell = cells_to_visit.front();
        cells_to_visit.pop_front();

        // если ячейка по позиции 'cell' была уже посещена, имеет место цикл
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
    // так как в этой ячейке либо изменилась формула, либо формулу поменяли на текст,
    // значение тоже должно быть инвалидировано
    cashed_value_.reset();

    for (CellInterface* icell : parents_) {
        Cell* cell = static_cast<Cell*>(icell);
        if (!cell->IsCacheInvalidated()) {
            cell->InvalidateCache();
        }
    }
}

void Cell::AddThisToChildren(const std::vector<Position>& referenced_cells) {
    for (const Position& pos : referenced_cells) {
        Cell* cell = static_cast<Cell*>(sheet_.GetCell(pos));
        cell->GetParentSet().insert(this);
    }
}

std::unordered_set<CellInterface*>& Cell::GetParentSet() {
    return parents_;
}

bool Cell::IsEmpty() const {
    return text_.size() == 0 && formula_ == nullptr;
}

void Cell::DeleteThisFromChildren(const std::vector<Position>& referenced_cells) {
    for (const Position& pos : referenced_cells) {
        Cell* cell = static_cast<Cell*>(sheet_.GetCell(pos));
        cell->GetParentSet().erase(this);
    }
}

bool Cell::IsCacheInvalidated() const {
    return !cashed_value_.has_value();
}