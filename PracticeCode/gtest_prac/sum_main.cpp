
#include <iostream>

//include the google test dependencies
#include <gtest/gtest.h>

/*
//declare the function(s) that you are testing
double summer(double[], int);

//our first unit test
TEST(IntegerInputsSuite, simpleSum)
{
  //first, set up any inputs to your
  const int SIZE = 3;
  double arr[SIZE]  = {1, 2, 3};
  //then, make an assertion to test
  EXPECT_EQ(summer(arr, SIZE), 6) << "The sum is not correct";
}

TEST(IntegerInputsSuite, oneElement)
{
  const int SIZE = 1;
  double arr[SIZE]  = {33};
  EXPECT_EQ(summer(arr, SIZE), 33) << "The sum is not correct for array of size 1";
}

TEST(DoubleInputsSuite, simpleSum)
{
  const int SIZE = 3;
  double arr[SIZE]  = {1.1, 1.1, 1};
  EXPECT_EQ(summer(arr, SIZE), 3.2) << "The sum is not correct using     double inputs";
}
*/

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


