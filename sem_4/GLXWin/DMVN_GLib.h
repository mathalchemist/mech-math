// DMVN X-Library. (C) DMVN Corporation, 2003
// Authors: Veltishchev Dmitry, Veltishchev Michail.
// Version 1.03 BETA, Build on Sunday, 16.02.2003
// This file contains useful class for working with X-Window applications.
// An XWin object, being declared as TXWin, connects to X-Server
// while constructing. At the same time it creates window with specified
// attributes. If X-Server is launched, but you are working on the console
// (TEXT screen mode), program will be halted, and this error cannot be
// handled properly. If X-Server wasn't initialized before running program,
// it MUST report about the error ("Cannot connect to X Server").
// To create window in any time you need, you can declare a pointer
// to class TXWin (for example, TXWin * XWin), and initialize it
// using operator NEW:
//
//	TXWin * XWin = new TXWin("Hello, world", 100, 100, 300, 200);
//
// and kill it using operator DELETE before exiting program:
//
// 	delete XWin;
//
// Don't forget to compile program with g++ compiler, because
// C language (but not C++) doesn't support NEW and DELETE operators.
// Compilation:
// g++ Program.cpp DMVN_XLib.cpp -Wall -O0 -I/usr/X11R6/include -L/usr/X11R6/lib
// -lX11 -o Program.exe
// Also add include file <DMVN_XLib.h> to your program.

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef __DMVN_GLIB_H_INCLUDED__
#define __DMVN_GLIB_H_INCLUDED__

typedef unsigned long DWORD;

enum RGBModes { RGB_555, RGB_565, RGB_888 };

DWORD RGB(int R, int G, int B);

enum EventTypes { ET_UNKNOWN, ET_EXPOSE, ET_KEYPRESS, ET_BUTTONPRESS };

enum MouseButtons {
  MB_UNKNOWN,
  MB_LEFT,
  MB_MIDDLE,
  MB_RIGHT,
  MB_MWHEELUP,
  MB_MWHEELDOWN
};

class TMouseInfo {
public:
  int X;
  int Y;
  int Button;
};

class TKeyInfo {
public:
  char KeyCode;
  DWORD KeyData;
};

class TXWin {
  bool GLSwap;
  bool GLContextCreated;
  XVisualInfo *GLVisual;
  GLXContext GLContext;
  Display *Disp;
  int Scr;
  Window Win;
  GC GContext;
  bool XInitialized;
  bool ErrorPresent;
  bool Initialized;

public:
  TXWin(char *Title, int X, int Y, int Width, int Height, int RGBMode);
  ~TXWin(void);
  bool Error(void) { return ErrorPresent; }
  void GetEvent(int *EventType, TKeyInfo *KeyInfo, TMouseInfo *MouseInfo);
  void DrawPoint(int X, int Y, DWORD Color);
  void DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color);
  void FillRect(int X, int Y, int Width, int Height);
};

#endif
