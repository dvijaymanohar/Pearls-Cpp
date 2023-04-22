

#include <iostream>

double summer(double arr[] , int size)
{
  double sum = 0;
  for(int i = 0; i < size; i++)
  {
    sum += arr[i];
  }
  return sum;
}
