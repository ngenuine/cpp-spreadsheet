#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

#include <optional>
#include <unordered_set>

class Impl {
public:
    virtual CellInterface::Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
    virtual void Clear() = 0;
    virtual bool IsEmpty() const = 0;
    virtual bool IsText() const = 0;
    virtual bool IsFormula() const = 0;
};

class EmptyImpl : public Impl {
public:
    CellInterface::Value GetValue() const override;
    std::string GetText() const override;
    void Clear() override;
    virtual bool IsEmpty() = 0;
    virtual bool IsText() = 0;
    virtual bool IsFormula() = 0;
};

class TextImpl : public Impl {
public:
    CellInterface::Value GetValue() const override;
    std::string GetText() const override;
    void Clear() override;
    virtual bool IsEmpty() = 0;
    virtual bool IsText() = 0;
    virtual bool IsFormula() = 0;
private:

};

class FormulaImpl : public Impl {
public:
    CellInterface::Value GetValue() const override;
    std::string GetText() const override;
    void Clear() override;
private:
    std::unique_ptr<FormulaInterface> formula_;
};

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    std::unordered_set<CellInterface*>& GetParentSet();
    bool CheckCycles(const std::vector<Position>& referenced_cells) const;
    void AddThisToChildren(const std::vector<Position>& referenced_cells);
    void DeleteThisFromChildren(const std::vector<Position>& referenced_cells);
    void InvalidateCache();
    bool IsCacheInvalidated() const;
    bool IsEmpty() const;
private:
    // std::unique_ptr<Impl> impl_;
    std::string text_;
    std::unique_ptr<FormulaInterface> formula_;

    // Подсказка:
    // Ячейки зависят друг от друга. Это напоминает структуру данных граф. Фактически ячейки образуют граф зависимостей.
    // Граф состоит из узлов и рёбер. В программах обычно определяют класс «узел» графа, содержащий набор указателей на другие узлы, с которыми он соединён рёбрами.
    // Если вы представите ячейки не только как элементы таблицы, но и как узлы графа, сможете находить циклические зависимости, инвалидировать кэш и более эффективно вычислять формулы.
    // как хранить граф -- будет метод GetReferencedCells, который возвращает зависимые ячейки.
    // граф судя по всему хранится в самой ячейке в виде множества указателей на ячейки в формуле
    // std::unordered_set<const CellInterface*> master_linked_cells_;
    std::unordered_set<CellInterface*> parents_;

    // в самой ячейке будет кэшированное значение, которое может быть ошибкой или числом
    // сбрасывать надо весь кэш по зависимостям обратно
    // если удалили/изменими ячейку, от которой зависят другие ячейки, менять cashed_value_ будет метод GetValue
    mutable std::optional<CellInterface::Value> cashed_value_;
    SheetInterface& sheet_;
};

std::unique_ptr<CellInterface> CreateCell(const std::string& str, const SheetInterface* sheet);