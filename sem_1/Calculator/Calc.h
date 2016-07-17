// Integer arithmetic with very big numbers (up to 10^MAX_LENGTH)
// (C) SEGMENTATION FAULT Software Inc, 2003

#ifndef __DMVN_CALC_H_INCLUDED__
#define __DMVN_CALC_H_INCLUDED__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_LENGTH 10000

class TNum
{
   char * Data;
public:
   TNum(void);
   TNum(int X);
   TNum(TNum& N);
   ~TNum(void);
   void Write(char * FileName);
   void Display(void);
   int GetLength(void);
   TNum& operator = (TNum& N);
   TNum& operator = (int N);
   bool operator == (TNum& N);
   bool operator == (int N);
   bool operator != (TNum& N);
   bool operator != (int N);
   TNum& operator ++ (int);
   TNum& operator ++ (void);
   TNum& operator -- (int);
   TNum& operator -- (void);
   TNum& operator += (TNum& N);
   TNum& operator += (int N);
   TNum& operator -= (TNum& N);
   TNum& operator -= (int N);

   TNum& operator *= (TNum& N);
   TNum& operator *= (int N);
};

#endif
