#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(std::function<const CellInterface*(std::string)> cell_getter) const;

    const std::forward_list<Position>& GetReferencedCells() const;

    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;
private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;

    // все встреченные индексы ячеек сохранятся в отдельный список при парсинге формулы в методе ParseFormulaAST
    std::forward_list<Position> referenced_cells_;
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);