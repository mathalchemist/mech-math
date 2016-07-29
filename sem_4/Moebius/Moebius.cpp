#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <X11/keysym.h>
#include "DMVN_XLib.h"

TXWin *XWin;

const int XWW = 500;
const int XWH = 270;

TKeyInfo KInfo;
TMouseInfo MInfo;

int KeyHandler();
void ExposeHandler();
bool Working = true;

// MAIN ==================================
int main(int argc, char **argv) {
  try {
    XWin = new TXWin("Moebius surface", 100, 100, XWW, XWH, RGB_565);
    while (true) {
      int EType;
      if (XWin->GetEvent(&EType, &KInfo, &MInfo)) {
        if (EType == ET_EXPOSE)
          ExposeHandler();
        if (EType == ET_KEYPRESS && KeyHandler() == 1)
          break;
      }
      //	    else NetworkHandler(s0);
    }
    delete XWin;
  } catch (const TXWin::Ex &e) {
    printf("Error: %s.\n", e.Msg);
    return 1;
  }
  return 0;
}

void ExposeHandler() {}

int KeyHandler() {
  if (KInfo.KeyCode == XK_Q)
    return 1; // quit
  return 0;
}
