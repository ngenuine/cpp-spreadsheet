#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        std::ostringstream out;
        out << '(' << pos.row << ", "s << pos.col << ')';
        throw InvalidPositionException("invalid position: "s + out.str());
    }

    if (text.size() > 0) {
        // std::cout << text << pos.row << ' ' << pos.col << std::endl;
        if (pos.row + 1 > static_cast<int>(data_.size())) {
            data_.resize(pos.row + 1);
        }

        if (pos.col + 1 > static_cast<int>(data_.at(pos.row).size())) {
            data_.at(pos.row).resize(pos.col + 1);
        }

        data_.at(pos.row).at(pos.col) = CreateCell(text);
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return GetCell(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        std::ostringstream out;
        out << '(' << pos.row << ", "s << pos.col << ')';
        throw InvalidPositionException("invalid position: "s + out.str());
    }

    try {
        return data_.at(pos.row).at(pos.col).get();
    } catch (const std::out_of_range& ex) { }

    return nullptr;
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
            // std::cout << "row counter: " << row_counter << ' ' << "col counter: " << col_counter << std::endl;
            const auto& cell = row.at(col_counter);
            // std::cout << cell->GetText() << std::endl;
            if (cell) { // cell
                if (size.rows < row_counter + 1) {
                    size.rows = row_counter + 1;
                }

                if (size.cols < col_counter + 1) {
                    size.cols = col_counter + 1;
                }
            }

            // std::cout << size.rows << ' ' << size.cols << std::endl;
        }
    }

    return size;
}

void Sheet::PrintValues(std::ostream& output) const {
    Size size = GetPrintableSize();

    for (int row = 0; row < size.rows; ++row) {
        bool is_first = true;
        bool is_last_char_is_tab = false;

        for (int col = 0; col < size.cols; ++col) {
            try {
                const std::unique_ptr<CellInterface>& cell = data_.at(row).at(col);

                if (cell) {
                    if (!is_first && !is_last_char_is_tab) {
                        output << '\t';
                    }

                    const auto& value = cell->GetValue();

                    if (std::holds_alternative<std::string>(value)) {
                        output << std::get<std::string>(value);
                    } else if (std::holds_alternative<double>(value)) {
                        output << std::get<double>(value);
                    } else {
                        output << std::get<FormulaError>(value);
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

void Sheet::PrintTexts(std::ostream& output) const {
    Size size = GetPrintableSize();

    for (int row = 0; row < size.rows; ++row) {
        bool is_first = true;
        bool is_last_char_is_tab = false;

        for (int col = 0; col < size.cols; ++col) {
            try {
                const std::unique_ptr<CellInterface>& cell = data_.at(row).at(col);
                
                if (cell) {
                    if (!is_first && !is_last_char_is_tab) {
                        output << '\t';
                    }

                    output << cell->GetText();

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