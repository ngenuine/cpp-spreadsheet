#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression);

    Value Evaluate(const SheetInterface& sheet) const override;

    std::string GetExpression() const override;

    std::vector<Position> GetReferencedCells() const override;
private:
    FormulaAST ast_;
};

Formula::Formula(std::string expression)
    : ast_(ParseFormulaAST(expression))
{
}

FormulaInterface::Value Formula::Evaluate(const SheetInterface& sheet) const {
    try {
        auto cell_getter = [&sheet](const Position* pos) { // может вернуть nullptr
            try {
                return sheet.GetCell(*pos);
            } catch (const InvalidPositionException& ex) {
                // если брошено исключение, то это из-за того, что
                // IsValid() сказал, что ячейка некорректная,
                // в этом месте некорректоной она может быть по причине
                // выхода за пределы листа
                auto category = FormulaError::Category::Ref;
                throw FormulaError(category);
            }
        };
        return ast_.Execute(cell_getter);
    } catch (const FormulaError& err) {
        return err;
    }
}

std::string Formula::GetExpression() const {
    std::ostringstream out;
    ast_.PrintFormula(out);

    return out.str();
}

std::vector<Position> Formula::GetReferencedCells() const {
    const std::forward_list<Position>& referenced_cells = ast_.GetCells();

    std::vector<Position> cells;
    for (const auto pos : referenced_cells) {
        cells.push_back(std::move(pos));
    }

    std::sort(cells.begin(), cells.end());
    auto last_unique = std::unique(cells.begin(), cells.end());
    cells.erase(last_unique, cells.end());

    return cells;
}

}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    return std::make_unique<Formula>(std::move(expression));
}