
#ifndef _ORDER_TABLE_H_
#define _ORDER_TABLE_H_

#include <memory>
#include <string>
#include <vector>

class Column {

public:
  enum ColumnType { String, Integer, Real, Date };

  Column(const std::string &name, ColumnType type);

  const std::string &Name() const;
  ColumnType Type() const;

private:
  std::string mName;
  ColumnType mType;
};

class TableValue {

public:
  TableValue();
  TableValue(const std::string &s);
  TableValue(int n);
  TableValue(double n);

  bool IsNull() const;

  const std::string &AsString() const;

private:
  std::string mVal;
  bool mIsNull;
};

typedef std::vector<TableValue> TableRow;

class Table {

public:
  Table();
  virtual ~Table();
  void Clear();

  void AddColumn(const Column &col);
  void AddRow(const TableRow &row);
  void ClearRows();

  const TableRow &Row(unsigned int row) const;
  TableValue &Value(unsigned int col, unsigned int row) const;

  unsigned int Depth() const; // Depth is number of rows in the table

private:
  std::vector<Column> mCols;
  std::vector<TableRow> mRows;
};

#endif