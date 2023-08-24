#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <sstream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        std::ostringstream out;
        out << '(' << pos.row << ", "s << pos.col << ')';
        throw InvalidPositionException("invalid position: "s + out.str());
    }

    if (pos.row + 1 > static_cast<int>(data_.size())) {
        data_.resize(pos.row + 1);
    }

    if (pos.col + 1 > static_cast<int>(data_.at(pos.row).size())) {
        data_.at(pos.row).resize(pos.col + 1);
    }

    // сохраним прежнюю ячейку, потому что если установка нового значения окажется
    // успешной (не вылетит FormulaException или CircularDependencyException), то
    // надо будет сходить по детям прежней ячейки и разорвать зависимость прежней ячейки от ее детей
    // чтобы однажды дети не обнулили кэш раньше зависящей от них ячейки
    auto old_cell = std::move(data_.at(pos.row).at(pos.col));

    data_.at(pos.row).at(pos.col) = std::make_unique<Cell>(*this);

    try { // попробуем записать формулу в ячейку
        // бросит FormulaException при синтаксически некорректной формуле
        // или CircularDependencyException если text несет в таблицу циклы
        data_.at(pos.row).at(pos.col)->Set(text);

        if (old_cell) {
            // разрываем зависимость родителя от его детей; если это не сделать,
            // у детей в parents будут указатели на погибших родителей
            // если в old_cell был текст, ничего не произойдет, т.к. детей не было
            static_cast<Cell*>(old_cell.get())->DeleteThisFromChildren();

            // если на прежнюю ячейку кто-то ссылался (родители), надо их в новую ячейку
            // которая пришла на эту позицию на замену прежней, перенести
            auto& actual_parents = static_cast<Cell*>(GetCell(pos))->GetParentSet();
            auto& previous_parents = static_cast<Cell*>(old_cell.get())->GetParentSet();
            actual_parents = previous_parents;
        }
        
        // создаем новую зависимость родителя от детей
        // если text не формула, ничего не произойдет, т.к. детей нет
        static_cast<Cell*>(GetCell(pos))->AddThisToChildren();

    } catch (const CircularDependencyException& ex) {

        // если оказалось, что формула несет циклы, откатим все назад
        data_.at(pos.row).at(pos.col) = std::move(old_cell);

        // и перевыбросим
        throw;
    }

    static_cast<Cell*>(GetCell(pos))->InvalidateCache();
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        std::ostringstream out;
        out << '(' << pos.row << ", "s << pos.col << ')';
        throw InvalidPositionException("invalid position: "s + out.str());
    }

    try {
        return data_.at(pos.row).at(pos.col).get();
    } catch (const std::out_of_range& ex) {

    }

    return nullptr;    
}

CellInterface* Sheet::GetCell(Position pos) {
    return const_cast<CellInterface*>(
              static_cast<const Sheet&>(*this).GetCell(pos)
           );
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        std::ostringstream out;
        out << '(' << pos.row << ", "s << pos.col << ')';
        throw InvalidPositionException("invalid position: "s + out.str());
    }
    
    try {
        data_.at(pos.row).at(pos.col).release();
    } catch (const std::out_of_range& ex) { }
}

Size Sheet::GetPrintableSize() const {
    Size size;

    for (int row_counter = 0; row_counter < static_cast<int>(data_.size()); ++row_counter) {
        const auto& row = data_.at(row_counter);
        for (int col_counter = 0; col_counter < static_cast<int>(row.size()); ++col_counter) {
            
            const std::unique_ptr<CellInterface>& cell = row.at(col_counter);

            if (cell && !static_cast<Cell*>(cell.get())->IsEmpty()) {
                if (size.rows < row_counter + 1) {
                    size.rows = row_counter + 1;
                }

                if (size.cols < col_counter + 1) {
                    size.cols = col_counter + 1;
                }
            }
        }
    }

    return size;
}

void Sheet::PrintValues(std::ostream& output) const {
    auto value_getter = [](const std::unique_ptr<CellInterface>& cell) {
        return cell->GetValue();
    };

    PrintSheet(output, value_getter);
}

void Sheet::PrintTexts(std::ostream& output) const {
    auto text_getter = [](const std::unique_ptr<CellInterface>& cell) {
        return cell->GetText();
    };

    PrintSheet(output, text_getter);
}

bool Sheet::IsEmptyRow(const std::vector<std::unique_ptr<CellInterface>>& row) const {
    return std::all_of(row.begin(), row.end(),
        [](const std::unique_ptr<CellInterface>& cell) {
            return !cell;
        }
    ); // вернет true если вектор row пустой
}

void Sheet::PrintSheet(std::ostream& output, std::function<CellInterface::Value(const std::unique_ptr<CellInterface>&)> getter) const {
    Size size = GetPrintableSize();

    for (int row = 0; row < size.rows; ++row) {
        bool is_first = true;
        bool is_last_char_is_tab = false;

        const std::vector<std::unique_ptr<CellInterface>>& table_row = data_.at(row);

        if (IsEmptyRow(table_row)) {
            output << '\t' << '\n';
            continue;
        }

        for (int col = 0; col < size.cols; ++col) {
            try {
                const std::unique_ptr<CellInterface>& cell = table_row.at(col);
                
                if (cell) {
                    if (!is_first && !is_last_char_is_tab) {
                        output << '\t';
                    }

                    const auto& result = getter(cell); // в PrintSheet передан такой функциональный объект, котрый вернет все что нужно

                    if (std::holds_alternative<std::string>(result)) {
                        output << std::get<std::string>(result);
                    } else if (std::holds_alternative<double>(result)) {
                        output << std::get<double>(result);
                    } else {
                        output << std::get<FormulaError>(result);
                    }

                    is_first = false;
                    is_last_char_is_tab = false;
                } else {
                    output << '\t';
                    is_last_char_is_tab = true;
                }
            } catch (const std::out_of_range& ex) {
                output << '\t';
                is_last_char_is_tab = true;
            }
        }

        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}