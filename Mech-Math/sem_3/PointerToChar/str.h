// =====================================
// Char|Int pointer|array analyser
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#ifndef __STR_H_INCLUDED__
#define __STR_H_INCLUDED__

#include <string.h>

class string
{
  public:
  char s[1000];
  string() { s[0]=0; }
  string(char * ss) { strcpy(s,ss); }
  string(string& ss) { strcpy(s,ss.s); }
  string& operator = (const char* ss) { strcpy(s, ss); return *this; }
  string& operator = (const string& ss) { strcpy(s, ss.s); return *this; }
  string& operator += (const string& ss) { strcat(s, ss.s); return *this; } 
};

#endif
