// DMVN_XLib example code.
// (C) SEGMENTATION FAULT Software, 2003

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "DMVN_XLib.h"

TKeyInfo KInfo;
TMouseInfo MInfo;

void ExposeHandler(void);

bool Working = true;
TXWin XWin = TXWin("TXWin class test window", 100, 100, 256, 256, RGB_565);

int main() {
  //    if (XWin.Error()) return 0;
  while (Working) {
    int EType;
    XWin.GetEvent(&EType, &KInfo, &MInfo);
    if (EType == ET_EXPOSE)
      ExposeHandler();
  }
}

// Application event handlers
void ExposeHandler(void) {
  for (double I = -128; I < 128; I += 0.1) {
    int Color = 0;
    int J = int(128 * sqrt(1 - (I / 128.0) * (I / 128.0)));
    XWin.DrawPoint(128 + int(I), 128 - int(J), RGB(Color, Color, Color));
    XWin.DrawPoint(128 + int(I), 128 - int(-J), RGB(Color, Color, Color));
  }
}
