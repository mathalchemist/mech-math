// Integer arithmetic with very big numbers (up to 10^MAX_LENGTH)
// (C) SEGMENTATION FAULT Software Inc, 2003

#include "Calc.h"

// main
int main() {
  TNum A(1);
  for (int i = 1; i <= 800; i++)
    A *= 5;
  A.Write("5^800.txt");
  return 0;
}
