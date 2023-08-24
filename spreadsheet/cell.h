#pragma once

#include "common.h"
#include "formula.h"

// class Impl {
// public:

// private:
//     std::string content_;
// };

// class EmptyImpl;
// class TextImpl;
// class FormulaImpl;


class Cell : public CellInterface {
public:
    Cell();
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;

private:
    // std::unique_ptr<Impl> impl_;
    std::string text_;
    std::unique_ptr<FormulaInterface> formula_;
};