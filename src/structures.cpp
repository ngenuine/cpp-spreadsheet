#include "common.h"

#include <cctype>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <vector>

// #include <iostream>

using namespace std::literals;

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

// Реализуйте методы:
bool Position::operator==(const Position rhs) const {
    return col == rhs.col && row == rhs.row;
}

bool Position::operator<(const Position rhs) const {
    return col < rhs.col || row < rhs.row;
}

bool Position::IsValid() const {
    bool is_non_negative = col >= 0 && row >= 0;
    return is_non_negative && col < MAX_COLS && row < MAX_ROWS;
}

std::string Position::ToString() const {
    if (!IsValid()) {
        return ""s;
    }

    int whole = col;
    bool is_first = true;

    // такая проблема вышла, что раз 0 у меня нет в системе счисления, то
    // будет отличие от того, впервые это посчитанный остаток или нет
    // к тому же раз остаток может быть 0, это будет означать как в кольцевом
    // буфере 0 - 1 = 25, или иначе, букву Z
    auto get_char = [&is_first](int r) {    
        if (is_first) { 
            is_first = false;
            return ALPHABET[r];
        } else if (r == 0) {
            return 'Z';
        }

        return ALPHABET[r - 1];
    };

    std::string remains;

    int acc = 0;    // буду сразу считать численное представление в моей системе счисления
    int power = 1;  // оно должно сравняться за вычетом 1 с переданным числовым представлением колонки

    while (acc <= col) {
        char r = get_char(whole % ALPHABET.size());

        acc += (r - 'A' + 1) * power;
        power *= ALPHABET.size();

        remains.push_back(r);
        whole /= ALPHABET.size();
    }

    std::reverse(remains.begin(), remains.end());
    remains += std::to_string(row + 1);

    return remains;
}

Position Position::FromString(std::string_view str) {
    if (str.size() > MAX_LENGTH) {
        return Position::NONE;
    }

    using It = std::string_view::iterator;

    auto is_num = [](char charecter) { return charecter >= '0' && charecter <= '9'; };
    auto is_uppercase_letter = [](char charecter) { return charecter >= 'A' && charecter <= 'Z'; };
    auto is_special_sumbol = [&is_num, &is_uppercase_letter](char character) {
        return !(is_num(character) || is_uppercase_letter(character));
    };

    auto get_first_num_pos = [&is_num](It begin, It end) {
        while (begin != end) {
            if (is_num(*begin)) { return begin; }
            ++begin;
        }

        return end;
    };

    auto fill_str = [is_special_sumbol](It begin, It end) {
        std::string str;
        while (begin != end) {
            if (is_special_sumbol(*begin)) {
                return ""s;
            }
            
            str.push_back(*begin);
            ++begin;
        }

        return str;
    };

    auto first_num_pos = get_first_num_pos(str.begin(), str.end());

    std::string col_as_string = fill_str(str.begin(), first_num_pos);
    std::string row_as_string = fill_str(first_num_pos, str.end());

    bool is_any_empty = col_as_string.size() * row_as_string.size() == 0;

    bool is_correct_col_as_string = std::all_of(col_as_string.begin(), col_as_string.end(),
                                        [&is_uppercase_letter](char c) { return is_uppercase_letter(c); });

    bool is_correct_row_as_string = std::all_of(row_as_string.begin(), row_as_string.end(),
                                        [&is_num](char c) { return is_num(c); });

    if (!is_correct_col_as_string || !is_correct_row_as_string || is_any_empty) {
        return Position::NONE;
    }

    std::reverse(col_as_string.begin(), col_as_string.end());
    int col_as_num = 0;
    int power = 1;
    for (char c : col_as_string) {
        col_as_num += (c - 'A' + 1) * power;
        power *= ALPHABET.size();
    }

    --col_as_num;
    int row_as_num = std::stoi(row_as_string) - 1;

    bool is_correct_col_as_num = col_as_num >= 0 && col_as_num < MAX_COLS;
    bool is_correct_row_as_num = row_as_num >= 0 && row_as_num < MAX_ROWS;

    if (!is_correct_col_as_num || !is_correct_row_as_num) {
        return Position::NONE;
    }


    return {row_as_num, col_as_num};
}