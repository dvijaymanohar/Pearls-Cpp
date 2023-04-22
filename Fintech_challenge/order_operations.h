#include <iostream>
#include <string>

#include "order_table.h"


class OrderOperations {
private:
  Table t;

public:
  OrderOperations() = default;
  ~OrderOperations() { t.Clear(); }

  // Get the data from the data source and populate the table.
  int PopulateTableData(const std::string filename);

  // Counts the total number of orders per symbol.
  void OrderCounts(void) const;

  // Finds the top 3 biggest BUY orders in terms of volume for a specific
  // symbol.
  void BiggestBuyOrders(std::string symbol = "DVAM1") const;

  // Finds the best SELL price and related order volume for a specific symbol
  void BestSellAtTime(std::string symbol = "DVAM1",
                      std::string timestamp = "15:30:00") const;
};