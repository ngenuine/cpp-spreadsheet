#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression);
    Value Evaluate() const override;
    std::string GetExpression() const override;

private:
    FormulaAST ast_;
};

Formula::Formula(std::string expression)
    : ast_(ParseFormulaAST(expression))
{
}

FormulaInterface::Value Formula::Evaluate() const {
    try {
        return ast_.Execute();
    } catch (const FormulaError& err) {
        return err;
    }
}

std::string Formula::GetExpression() const {
    std::ostringstream out;
    ast_.PrintFormula(out);

    return out.str();
}

}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}