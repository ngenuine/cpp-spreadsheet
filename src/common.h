/* common.h
Объявления вспомогательных структур.
 */

#pragma once


#include <sstream>
#include <string>
#include <string_view>

using namespace std::literals;

// Позиция ячейки. Индексация с нуля.
struct Position {
    int row = 0;
    int col = 0;

    bool operator==(Position rhs) const;
    bool operator<(Position rhs) const;

    bool IsValid() const;
    std::string ToString() const;

    static Position FromString(std::string_view str);

    static const int MAX_ROWS = 16384;
    static const int MAX_COLS = 16384;
    static const Position NONE;
    inline static const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"s;
    inline static const std::string NUMBERS = "0123456789"s;
    static const int MAX_LENGTH = 8;
};