#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

#include <optional>
#include <unordered_set>

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    std::unordered_set<Cell*>& GetParentSet();
    
    bool CheckCycles(const std::vector<Position>& referenced_cells) const;
    
    void AddThisToChildren();
    void DeleteThisFromChildren();
    
    void InvalidateCache();
    bool IsCacheInvalidated() const;
    
    bool IsFormula() const;
    bool IsEmpty() const;
private:
    std::string text_;
    std::unique_ptr<FormulaInterface> formula_;

    std::unordered_set<Cell*> parents_;

    mutable std::optional<CellInterface::Value> cashed_value_;
    SheetInterface& sheet_;
};