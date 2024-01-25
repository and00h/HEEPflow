#include <stdio.h>
#include "lenet5_test.h"

int main() {
  int a = RunLenet5();
  printf("Execution terminated with code %d\r\n", a);
  return a;
}