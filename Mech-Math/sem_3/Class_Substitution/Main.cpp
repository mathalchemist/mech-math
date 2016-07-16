// =====================================
// Substitution class
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include "subst.h"
#include "stdio.h"

int main()
{
  // substitution test
  const int max = 10;
  int s[max];
  
  for (int i =0; i < max;i++) s[i] = (i+1)%max;
  
  TSubst S(s, max);

  for (int i=0; i< 3; i++) { S *= S; S.Print(); }

  return 0;

}
