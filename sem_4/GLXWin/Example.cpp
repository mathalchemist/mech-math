// DMVN_XLib example code.
// (C) SEGMENTATION FAULT Software, 2003

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "./../DMVN_XLib.h"

void ExposeHandler(void);
void KeyboardHandler(TKeyInfo KInfo);
void MouseHandler(TMouseInfo MInfo);

bool Working = true;
TXWin XWin = TXWin("TXWin class test window", 100, 100, 256, 256, RGB_565);

int main() {
  if (XWin.Error())
    return 0;
  while (Working) {
    int EType;
    TMouseInfo MInfo;
    TKeyInfo KInfo;
    XWin.GetEvent(&EType, &KInfo, &MInfo);

    switch (EType) {
    case ET_EXPOSE:
      ExposeHandler();
      break;
    case ET_KEYPRESS:
      KeyboardHandler(KInfo);
      break;
    case ET_BUTTONPRESS:
      MouseHandler(MInfo);
      break;
    default:
      printf("Unknown event occured.\n");
    }
  }
}

// Application event handlers
void ExposeHandler(void) {
  for (int I = 0; I < 256; I++)
    for (int J = 0; J < 256; J++) {
      int Color = int(
          255 * cos(0.1 * sqrt((I - 128) * (I - 128) + (J - 128) * (J - 128))));
      XWin.DrawPoint(I, J, RGB(Color, Color, Color));
    }
}

void KeyboardHandler(TKeyInfo KInfo) {
  switch (KInfo.KeyCode) {
  case 27:
    Working = false;
    break;
  default:
    XWin.FillRect(0, 0, 256, 256);
    break;
  }
}

void MouseHandler(TMouseInfo MInfo) {
  switch (MInfo.Button) {
  case MB_LEFT:
    XWin.DrawPoint(MInfo.X, MInfo.Y, RGB(0, 0, 0));
    break;
  case MB_RIGHT:
    XWin.DrawPoint(MInfo.X, MInfo.Y, RGB(128, 128, 128));
    break;
  case MB_MWHEELUP:
    XWin.DrawPoint(MInfo.X, MInfo.Y, RGB(0, 255, 0));
    break;
  case MB_MWHEELDOWN:
    XWin.DrawPoint(MInfo.X, MInfo.Y, RGB(0, 0, 255));
    break;
  }
}
