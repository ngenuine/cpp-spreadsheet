#include "common.h"
#include "test_runner_p.h"

inline std::ostream& operator<<(std::ostream& output, Position pos) {
    return output << "(" << pos.row << ", " << pos.col << ")";
}

inline Position operator"" _pos(const char* str, std::size_t) {
    return Position::FromString(str);
}

inline std::ostream& operator<<(std::ostream& output, Size size) {
    return output << "(" << size.rows << ", " << size.cols << ")";
}

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value) {
    std::visit(
        [&](const auto& x) {
            output << x;
        },
        value);
    return output;
}

namespace {

void TestEmpty() {
    auto sheet = CreateSheet();
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
}

void TestInvalidPosition() {
    auto sheet = CreateSheet();
    try {
        sheet->SetCell(Position{-1, 0}, "");
    } catch (const InvalidPositionException&) {
    }
    try {
        sheet->GetCell(Position{0, -2});
    } catch (const InvalidPositionException&) {
    }
    try {
        sheet->ClearCell(Position{Position::MAX_ROWS, 0});
    } catch (const InvalidPositionException&) {
    }
}

void TestSetCellPlainText() {
    auto sheet = CreateSheet();

    auto checkCell = [&](Position pos, std::string text) {
        sheet->SetCell(pos, text);
        CellInterface* cell = sheet->GetCell(pos);
        ASSERT(cell != nullptr);
        ASSERT_EQUAL(cell->GetText(), text);
        ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), text);
    };


    checkCell("A1"_pos, "Hello");
    checkCell("A1"_pos, "World");
    checkCell("B2"_pos, "Purr");
    checkCell("A3"_pos, "Meow");

    // Segmentation Fault
    // const SheetInterface& constSheet = *sheet;
    // ASSERT_EQUAL(constSheet.GetCell("B2"_pos)->GetText(), "Purr");

    sheet->SetCell("A3"_pos, "'=escaped");
    CellInterface* cell = sheet->GetCell("A3"_pos);
    ASSERT_EQUAL(cell->GetText(), "'=escaped");
    ASSERT_EQUAL(std::get<std::string>(cell->GetValue()), "=escaped");
}

void TestClearCell() {
    auto sheet = CreateSheet();

    sheet->SetCell("C2"_pos, "Me gusta");
    sheet->ClearCell("C2"_pos);
    ASSERT(sheet->GetCell("C2"_pos) == nullptr);

    sheet->ClearCell("A1"_pos);
    sheet->ClearCell("J10"_pos);
}
void TestPrint() {
    auto sheet = CreateSheet();
    sheet->SetCell("A2"_pos, "meow");
    sheet->SetCell("B2"_pos, "=1+2");
    sheet->SetCell("A1"_pos, "=1/0");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 2}));

    std::ostringstream texts;
    sheet->PrintTexts(texts);
    ASSERT_EQUAL(texts.str(), "=1/0\t\nmeow\t=1+2\n");

    std::ostringstream values;
    sheet->PrintValues(values);
    ASSERT_EQUAL(values.str(), "#DIV/0!\t\nmeow\t3\n");

    sheet->ClearCell("B2"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 1}));
}

void MyTestGetPrintableSize() {
    auto sheet = CreateSheet();
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
    sheet->SetCell("A1"_pos, "text1");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{1, 1}));
    sheet->SetCell("B2"_pos, "text1");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 2}));
    sheet->SetCell("C3"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{3, 3}));
    sheet->SetCell("D4"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{4, 4}));
    sheet->SetCell("E5"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{5, 5}));
    sheet->SetCell("F6"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{6, 6}));
    sheet->SetCell("G7"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{7, 7}));
    sheet->SetCell("H8"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{8, 8}));
    sheet->SetCell("I9"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{9, 9}));
    sheet->SetCell("J10"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{10, 10}));
    sheet->SetCell("K11"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{11, 11}));
    sheet->SetCell("L12"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{12, 12}));
    sheet->SetCell("M13"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{13, 13}));
    sheet->SetCell("N14"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{14, 14}));
    sheet->SetCell("O15"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{15, 15}));
    sheet->SetCell("P16"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{16, 16}));
    sheet->SetCell("Q17"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{17, 17}));
    sheet->SetCell("R18"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{18, 18}));
    sheet->SetCell("S19"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{19, 19}));
    sheet->SetCell("T20"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{20, 20}));
    sheet->SetCell("U21"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{21, 21}));
    sheet->SetCell("V22"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{22, 22}));
    sheet->SetCell("W23"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{23, 23}));
    sheet->SetCell("X24"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{24, 24}));
    sheet->SetCell("Y25"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{25, 25}));
    sheet->SetCell("Z26"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{26, 26}));
    sheet->SetCell("AA27"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{27, 27}));
    sheet->SetCell("AB28"_pos, "text2");
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{28, 28}));

    sheet->ClearCell("AB28"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{27, 27}));
    sheet->ClearCell("AA27"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{26, 26}));
    sheet->ClearCell("Z26"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{25, 25}));
    sheet->ClearCell("Y25"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{24, 24}));
    sheet->ClearCell("X24"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{23, 23}));
    sheet->ClearCell("W23"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{22, 22}));
    sheet->ClearCell("V22"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{21, 21}));
    sheet->ClearCell("U21"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{20, 20}));
    sheet->ClearCell("T20"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{19, 19}));
    sheet->ClearCell("S19"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{18, 18}));
    sheet->ClearCell("R18"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{17, 17}));
    sheet->ClearCell("Q17"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{16, 16}));
    sheet->ClearCell("P16"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{15, 15}));
    sheet->ClearCell("O15"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{14, 14}));
    sheet->ClearCell("N14"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{13, 13}));
    sheet->ClearCell("M13"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{12, 12}));
    sheet->ClearCell("L12"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{11, 11}));
    sheet->ClearCell("K11"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{10, 10}));
    sheet->ClearCell("J10"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{9, 9}));
    sheet->ClearCell("I9"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{8, 8}));
    sheet->ClearCell("H8"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{7, 7}));
    sheet->ClearCell("G7"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{6, 6}));
    sheet->ClearCell("F6"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{5, 5}));
    sheet->ClearCell("E5"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{4, 4}));
    sheet->ClearCell("D4"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{3, 3}));
    sheet->ClearCell("C3"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{2, 2}));
    sheet->ClearCell("B2"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{1, 1}));
    sheet->ClearCell("A1"_pos);
    ASSERT_EQUAL(sheet->GetPrintableSize(), (Size{0, 0}));
}

void MyTestPrintText1() {
    auto sheet = CreateSheet();

    sheet->SetCell("O1"_pos, "");

    sheet->SetCell("A1"_pos, "a1");
    sheet->SetCell("B1"_pos, "b1");
    sheet->SetCell("C1"_pos, "c1");
    sheet->SetCell("D1"_pos, "d1");
    sheet->SetCell("E1"_pos, "e1");
    sheet->SetCell("F1"_pos, "f1");
    sheet->SetCell("G1"_pos, "g1");

    // sheet->SetCell("A2"_pos, "a");
    // sheet->SetCell("B2"_pos, "a");
    sheet->SetCell("C2"_pos, "c2");
    sheet->SetCell("D2"_pos, "d2");
    sheet->SetCell("E2"_pos, "e2");
    sheet->SetCell("F2"_pos, "f2");
    sheet->SetCell("G2"_pos, "g2");

    sheet->SetCell("A3"_pos, "a3");
    sheet->SetCell("B3"_pos, "b3");
    sheet->SetCell("C3"_pos, "c3");
    sheet->SetCell("D3"_pos, "d3");
    sheet->SetCell("E3"_pos, "e3");
    // sheet->SetCell("F3"_pos, "a");
    // sheet->SetCell("G3"_pos, "a");

    // sheet->SetCell("A4"_pos, "a");
    // sheet->SetCell("B4"_pos, "a");
    // sheet->SetCell("C4"_pos, "a");
    // sheet->SetCell("D4"_pos, "a");
    // sheet->SetCell("E4"_pos, "a");
    // sheet->SetCell("F4"_pos, "a");
    // sheet->SetCell("G4"_pos, "a");

    sheet->SetCell("A5"_pos, "a5");
    // sheet->SetCell("B5"_pos, "a");
    sheet->SetCell("C5"_pos, "c5");
    // sheet->SetCell("D5"_pos, "a");
    sheet->SetCell("E5"_pos, "e5");
    // sheet->SetCell("F5"_pos, "a");
    sheet->SetCell("G5"_pos, "g5");

    // sheet->SetCell("A6"_pos, "a");
    sheet->SetCell("B6"_pos, "b6");
    // sheet->SetCell("C6"_pos, "a");
    sheet->SetCell("D6"_pos, "d6");
    // sheet->SetCell("E6"_pos, "a");
    sheet->SetCell("F6"_pos, "f6");
    // sheet->SetCell("G6"_pos, "a");

    sheet->SetCell("A7"_pos, "a7");
    sheet->SetCell("B7"_pos, "b7");
    // sheet->SetCell("C7"_pos, "a");
    // sheet->SetCell("D7"_pos, "a");
    sheet->SetCell("E7"_pos, "e7");
    sheet->SetCell("F7"_pos, "f7");
    // sheet->SetCell("G7"_pos, "a");

    // sheet->SetCell("A8"_pos, "a");
    // sheet->SetCell("B8"_pos, "a");
    sheet->SetCell("C8"_pos, "c8");
    sheet->SetCell("D8"_pos, "d8");
    // sheet->SetCell("E8"_pos, "a");
    // sheet->SetCell("F8"_pos, "a");
    sheet->SetCell("G8"_pos, "g8");

    sheet->SetCell("A9"_pos, "a9");
    sheet->SetCell("B9"_pos, "b9");
    sheet->SetCell("C9"_pos, "c9");
    // sheet->SetCell("D9"_pos, "a");
    // sheet->SetCell("E9"_pos, "a");
    // sheet->SetCell("F9"_pos, "a");
    // sheet->SetCell("G9"_pos, "a");

    // sheet->SetCell("A10"_pos, "a");
    // sheet->SetCell("B10"_pos, "a");
    // sheet->SetCell("C10"_pos, "a");
    sheet->SetCell("D10"_pos, "d10");
    sheet->SetCell("E10"_pos, "e10");
    sheet->SetCell("F10"_pos, "f10");
    sheet->SetCell("G10"_pos, "g10");

    sheet->PrintTexts(std::cout);
}

void MyTestPrintText2() {
    auto sheet = CreateSheet();

    sheet->SetCell("O1"_pos, "o1");

    sheet->SetCell("A1"_pos, "a1");
    sheet->SetCell("B1"_pos, "b1");
    sheet->SetCell("C1"_pos, "c1");
    sheet->SetCell("D1"_pos, "d1");
    sheet->SetCell("E1"_pos, "e1");
    sheet->SetCell("F1"_pos, "f1");
    sheet->SetCell("G1"_pos, "g1");

    // sheet->SetCell("A2"_pos, "a");
    // sheet->SetCell("B2"_pos, "a");
    sheet->SetCell("C2"_pos, "c2");
    sheet->SetCell("D2"_pos, "d2");
    sheet->SetCell("E2"_pos, "e2");
    sheet->SetCell("F2"_pos, "f2");
    sheet->SetCell("G2"_pos, "g2");

    sheet->SetCell("A3"_pos, "a3");
    sheet->SetCell("B3"_pos, "b3");
    sheet->SetCell("C3"_pos, "c3");
    sheet->SetCell("D3"_pos, "d3");
    sheet->SetCell("E3"_pos, "e3");
    // sheet->SetCell("F3"_pos, "a");
    // sheet->SetCell("G3"_pos, "a");

    // sheet->SetCell("A4"_pos, "a");
    // sheet->SetCell("B4"_pos, "a");
    // sheet->SetCell("C4"_pos, "a");
    // sheet->SetCell("D4"_pos, "a");
    // sheet->SetCell("E4"_pos, "a");
    // sheet->SetCell("F4"_pos, "a");
    // sheet->SetCell("G4"_pos, "a");

    sheet->SetCell("A5"_pos, "a5");
    // sheet->SetCell("B5"_pos, "a");
    sheet->SetCell("C5"_pos, "c5");
    // sheet->SetCell("D5"_pos, "a");
    sheet->SetCell("E5"_pos, "e5");
    // sheet->SetCell("F5"_pos, "a");
    sheet->SetCell("G5"_pos, "g5");

    // sheet->SetCell("A6"_pos, "a");
    sheet->SetCell("B6"_pos, "b6");
    // sheet->SetCell("C6"_pos, "a");
    sheet->SetCell("D6"_pos, "d6");
    // sheet->SetCell("E6"_pos, "a");
    sheet->SetCell("F6"_pos, "f6");
    // sheet->SetCell("G6"_pos, "a");

    sheet->SetCell("A7"_pos, "a7");
    sheet->SetCell("B7"_pos, "b7");
    // sheet->SetCell("C7"_pos, "a");
    // sheet->SetCell("D7"_pos, "a");
    sheet->SetCell("E7"_pos, "e7");
    sheet->SetCell("F7"_pos, "f7");
    // sheet->SetCell("G7"_pos, "a");

    // sheet->SetCell("A8"_pos, "a");
    // sheet->SetCell("B8"_pos, "a");
    sheet->SetCell("C8"_pos, "c8");
    sheet->SetCell("D8"_pos, "d8");
    // sheet->SetCell("E8"_pos, "a");
    // sheet->SetCell("F8"_pos, "a");
    sheet->SetCell("G8"_pos, "g8");

    sheet->SetCell("A9"_pos, "a9");
    sheet->SetCell("B9"_pos, "b9");
    sheet->SetCell("C9"_pos, "c9");
    // sheet->SetCell("D9"_pos, "a");
    // sheet->SetCell("E9"_pos, "a");
    // sheet->SetCell("F9"_pos, "a");
    // sheet->SetCell("G9"_pos, "a");

    // sheet->SetCell("A10"_pos, "a");
    // sheet->SetCell("B10"_pos, "a");
    // sheet->SetCell("C10"_pos, "a");
    sheet->SetCell("D10"_pos, "d10");
    sheet->SetCell("E10"_pos, "e10");
    sheet->SetCell("F10"_pos, "f10");
    sheet->SetCell("G10"_pos, "g10");

    sheet->PrintTexts(std::cout);
}

}  // namespace

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestEmpty);
    RUN_TEST(tr, TestInvalidPosition);
    RUN_TEST(tr, TestSetCellPlainText);
    RUN_TEST(tr, TestClearCell);
    RUN_TEST(tr, TestPrint);
    // std::cout << "All Yandex tests are passed" << std::endl;

    // RUN_TEST(tr, MyTestGetPrintableSize);
    // RUN_TEST(tr, MyTestPrintText1);
    // std::cout << '\n' << '\n' << '\n' << '\n' << '\n' << std::endl;
    // RUN_TEST(tr, MyTestPrintText2);

    // std::cout << "All My tests are passed" << std::endl;
    return 0;
}
