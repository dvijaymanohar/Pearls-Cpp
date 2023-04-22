#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>
#include <sys/types.h>

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <map>
#include <sstream>
#include <vector>

#include "order_table.h"

using namespace std;

vector<string> split(const string &s, char delim) {
  vector<string> result;
  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    result.push_back(item);
  }

  return result;
}

class OrderOperations {
private:
  Table t;
  enum colum_pos {
    TIMESTAMP,
    SYMBOL,
    ORDER_ID,
    OPERATION,
    SIDE,
    VOLUME,
    PRICE,
  };

public:
  OrderOperations() = default;
  ~OrderOperations() { t.Clear(); }

  // Get the data from the data source and populate the table.
  int PopulateTableData(const string filename);

  // Counts the total number of orders per symbol.
  void OrderCounts(void) const;

  // Finds the top 3 biggest BUY orders in terms of volume for a specific
  // symbol.
  void BiggestBuyOrders(string symbol = "DVAM1") const;

  // Finds the best SELL price and related order volume for a specific symbol
  void BestSellAtTime(string symbol = "DVAM1",
                      string timestamp = "15:30:00") const;
};

// Get the data from the data source and populate the table.
int OrderOperations::PopulateTableData(const string filename) {
  if (filename == "")
    return -1;

  ifstream input_file(filename);

  if (!input_file.is_open()) {
    cerr << "Could not open the orders file - '" << filename << "'"
         << std::endl;
    system("PAUSE");
    exit(EXIT_FAILURE);
  }

  t.AddColumn(Column("timestamp", Column::String));
  t.AddColumn(Column("symbol", Column::Integer));
  t.AddColumn(Column("order-id", Column::String));
  t.AddColumn(Column("operation", Column::Integer));
  t.AddColumn(Column("side", Column::String));
  t.AddColumn(Column("volume", Column::Integer));
  t.AddColumn(Column("price", Column::String));

  for (std::string line; getline(input_file, line);) {

    vector<string> v = split(line, ';');

    TableRow row;

    for (auto i : v)
      row.push_back(TableValue(i));

    t.AddRow(row);
  }

  input_file.close();

  return 0;
}

void OrderOperations::OrderCounts(void) const {
  uint32_t no_of_orders = t.Depth();

  if (no_of_orders == 0) {
    std::cout << "Data table is empty" << std::endl;
    return;
  }

  map<std::string, uint32_t> order_map;
  map<std::string, uint32_t>::iterator it;
  pair<map<std::string, uint32_t>::iterator, bool> ptr;

  for (uint32_t i = 0; i < no_of_orders; i++) {
    string sym = t.Value(SYMBOL, i).AsString();

    // Check if the symbol was already inserted
    it = order_map.find(sym);

    if (it == order_map.end()) {
      ptr = order_map.insert({sym, 1});
      if (!ptr.second)
        cout << "The key was newly inserted";
    } else {
      it->second = order_map[sym] + 1;
    }
  }

  std::cout << "No. of orders per symbol\n" << std::endl;

  cout << '\t' << "Symbol Name" << std::setw(25) << "No. of Orders"
       << std::endl;
  for (it = order_map.begin(); it != order_map.end(); ++it) {
    cout << '\t' << it->first << std::setw(25) << it->second << std::endl;
  }
}

template <typename A, typename B>
std::pair<B, A> flip_pair(const std::pair<A, B> &p) {
  return std::pair<B, A>(p.second, p.first);
}

template <typename A, typename B>
std::multimap<B, A> flip_map(const std::map<A, B> &src) {
  std::multimap<B, A> dst;
  std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()),
                 flip_pair<A, B>);
  return dst;
}

void OrderOperations::BiggestBuyOrders(string symbol /* = "DVAM1" */) const {
  uint32_t no_of_orders = t.Depth();
  const uint32_t max_display_orders = 2;

  map<uint32_t, uint32_t, std::greater<uint32_t>> volume_map;

  // Make a map with the location and volume by finding the BUY orders with the
  // given symbol
  for (uint32_t index = 0; index < no_of_orders; index++) {
    string symbolOfInterest = t.Value(SYMBOL, index).AsString();

    if (!symbolOfInterest.compare(symbol)) {
      if ("BUY" == t.Value(SIDE, index).AsString()) {
        const string volStr = t.Value(VOLUME, index).AsString();
        uint32_t vol = std::stoi(volStr);

        volume_map.insert({vol, index});
      }
    }
  }

  int j = 0;
  for (const auto &p : volume_map) {
    for (int col = TIMESTAMP; col <= PRICE; col++)
      cout << t.Value(col, p.second).AsString() << '\t';

    std::cout << std::endl;

    j++;
    if (j > max_display_orders)
      break;
  }
}

void OrderOperations::BestSellAtTime(string symbol /* = "DVAM1"*/,
                                     string timestamp /*= "15:30:00"*/) const {
  uint32_t no_of_orders = t.Depth();
  map<uint32_t, uint32_t, std::greater<uint32_t>> volume_map;
  struct std::tm tm = {0};

  std::istringstream ss(timestamp);
  ss >> std::get_time(&tm, "%H:%M:%S");
  std::time_t GivenTime = mktime(&tm);

  // Make a map with the location and volume by finding the BUY orders with the
  // given symbol
  for (uint32_t index = 0; index < no_of_orders; index++) {
    string symbolOfInterest = t.Value(SYMBOL, index).AsString();

    if (!symbolOfInterest.compare(symbol)) {
      if ("SELL" == t.Value(SIDE, index).AsString()) {
        string TStr = t.Value(TIMESTAMP, index).AsString();
        string TimeStr = TStr.substr(0, TStr.find('.'));

        std::istringstream ss(TimeStr);
        ss >> std::get_time(&tm, "%H:%M:%S");
        std::time_t OrderTimeStamp = mktime(&tm);

        const auto diff2 = std::difftime(GivenTime, OrderTimeStamp);
        if (diff2 == 0.0) {
          // std::cout << TimeStr << std::endl;

          const string volStr = t.Value(VOLUME, index).AsString();
          uint32_t vol = std::stoi(volStr);

          volume_map.insert({vol, index});
        }
      }
    }
  }

  for (const auto &p : volume_map) {
    for (int col = TIMESTAMP; col <= PRICE; col++)
      cout << t.Value(col, p.second).AsString() << '\t';

    std::cout << std::endl;
    break;
  }
}
