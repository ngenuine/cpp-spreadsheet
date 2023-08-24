#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

using namespace std::literals;

// Реализуйте следующие методы
Cell::Cell() = default;

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    Clear(); // если в ячейке что-то есть, то теперь этого не должно быть

    if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        // бросит FormulaException при синтаксически некорректной формуле
        formula_ = ParseFormula(std::string(text.begin() + 1, text.end()));
    } else {
        text_ = std::move(text);
    }
}

void Cell::Clear() {
    text_ = ""s;
    formula_.release();
}

Cell::Value Cell::GetValue() const {
    auto cut_escape_sign = [](const std::string& str) {
        return std::string(str.begin() + 1, str.end());
    };

    if (text_.size() > 0) { // значит ячейка содержит текст
        switch (text_[0])
        {
        case ESCAPE_SIGN:
            return cut_escape_sign(text_);
            break;
        default:
            return GetText();
            break;
        }
    }

    // значит ячейка содержит формулу

    auto result = formula_->Evaluate();
    if (std::holds_alternative<double>(result)) {
        return std::get<double>(result);
    }

    return std::get<FormulaError>(result);
}

std::string Cell::GetText() const {
    if (text_.size() > 0) { // значит ячейка содержит текст
        return text_;
    }

    return FORMULA_SIGN + formula_->GetExpression();
}