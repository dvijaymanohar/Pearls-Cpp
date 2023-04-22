#include "order_operations.h"
#include "order_table.h"
#include <iostream>

#include <ctime>
#include <iomanip>
#include <sstream>

int main(int argc, char *argv[]) {
  OrderOperations orderOps;

  if (0 != orderOps.PopulateTableData("orders.dat")) {
    std::cout << "Error populating the table using the data source"
              << std::endl;
    return EXIT_FAILURE;
  }

  orderOps.OrderCounts();

  orderOps.BiggestBuyOrders("DVAM1");
  orderOps.BiggestBuyOrders("TEST3");

  orderOps.BestSellAtTime("DVAM1", "13:49:34");
  orderOps.BestSellAtTime("DVAM1", "15:30:00");

  return EXIT_SUCCESS;
}