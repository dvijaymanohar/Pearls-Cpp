#include "order_table.h"
#include <iostream>

#include <bitset>
#include <climits>
#include <sstream>

using namespace std;

using std::string;

bool Equal(const string &lhs, const string &rhs) {
  unsigned int nl = lhs.size(), rl = rhs.size();
  if (nl != rl) {
    return false;
  }
  while (nl--) {
    if (toupper(lhs[nl]) != toupper(rhs[nl])) {
      return false;
    }
  }
  return true;
}

template <class TYPE> std::string Str(const TYPE &t) {
  std::ostringstream os;
  os << t;
  return os.str();
}

Column ::Column(const string &name, ColumnType type)
    : mName(name), mType(type) {}

const string &Column ::Name() const { return mName; }

Column::ColumnType Column ::Type() const { return mType; }

TableValue ::TableValue() : mVal(""), mIsNull(true) {}

TableValue ::TableValue(const string &s) : mVal(s), mIsNull(false) {}
TableValue ::TableValue(int n) : mVal(Str(n)), mIsNull(false) {}
TableValue ::TableValue(double n) : mVal(Str(n)), mIsNull(false) {}

bool TableValue ::IsNull() const { return mIsNull; }

const std::string &TableValue ::AsString() const {
  if (mIsNull) {
    static string NullVal = "NULL";
    return NullVal;
  } else {
    return mVal;
  }
}

Table ::Table() {}

Table ::~Table() { Clear(); }

void Table ::Clear() {
  mCols.clear();
  ClearRows();
}

void Table ::AddColumn(const Column &col) { mCols.push_back(col); }

void Table ::AddRow(const TableRow &row) {
  if (row.size() != mCols.size()) {
    std::cout << "Row and columns sizes do not match" << std::endl;
  }
  mRows.push_back(row);
}

// Remove all rows
void Table ::ClearRows() { mRows.clear(); }

// Get ref to row at row index
const TableRow &Table ::Row(unsigned int row) const {
  if (row >= mRows.size()) {
    std::cout << "Row index " << row << " out of range" << std::endl;
  }
  return mRows[row];
}

// Get value at column/row
TableValue &Table ::Value(unsigned int col, unsigned int row) const {
  if (col >= mCols.size()) {
    std::cout << "Column index " << col << " out of range" << std::endl;
  }

  return const_cast<TableRow &>(Row(row))[col];
}

// Depth is number of rows
unsigned int Table ::Depth() const { return mRows.size(); }
