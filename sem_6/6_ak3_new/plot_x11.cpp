#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include "plot_x11.h"

const int XPos = 0;
const int YPos = 0;
const int WDef = 800;
const int HDef = 600;
const int WMin = 50;
const int HMin = 50;
const int WBorder = 5;

const char *WTitle =
    "Function interpolation: Cubic interpolation using Akim's method";
const char *WITitle = WTitle;
const char *PClass = "FI_Class";

int SetWindowManagerHints(Display *prDisplay, const char *psPrgClass,
                          Window nWnd, int nMinWidth, int nMinHeight,
                          const char *psTitle, const char *psIconTitle,
                          Pixmap nIconPixmap) {
  XSizeHints rSizeHints;

  XWMHints rWMHints;
  XClassHint rClassHint;
  XTextProperty prWindowName, prIconName;

  if (!XStringListToTextProperty((char **)&psTitle, 1, &prWindowName) ||
      !XStringListToTextProperty((char **)&psIconTitle, 1, &prIconName))
    return 1; /* No memory! */

  rSizeHints.flags = PPosition | PSize | PMinSize;
  rSizeHints.min_width = nMinWidth;
  rSizeHints.min_height = nMinHeight;

  rWMHints.flags = StateHint | IconPixmapHint | InputHint;
  rWMHints.initial_state = NormalState;
  rWMHints.input = True;
  rWMHints.icon_pixmap = nIconPixmap;

  rClassHint.res_name = 0;
  rClassHint.res_class = (char *)psPrgClass;

  XSetWMProperties(prDisplay, nWnd, &prWindowName, &prIconName, 0, 0,
                   &rSizeHints, &rWMHints, &rClassHint);

  return 0;
}

#define XANC(c) XAllocNamedColor(prDisplay, colormap, c, &rColor, &rColorBase)

int AllocColors(Display *prDisplay, Colormap colormap, COLORS *colors) {
  XColor rColor;
  XColor rColorBase;

  if (!XANC("black"))
    return 1;
  colors->black = rColor.pixel;
  if (!XANC("blue"))
    return 2;
  colors->blue = rColor.pixel;
  if (!XANC("green"))
    return 3;
  colors->green = rColor.pixel;
  if (!XANC("cyan"))
    return 4;
  colors->cyan = rColor.pixel;
  if (!XANC("red"))
    return 5;
  colors->red = rColor.pixel;
  if (!XANC("magenta"))
    return 6;
  colors->magenta = rColor.pixel;
  if (!XANC("brown"))
    return 7;
  colors->brown = rColor.pixel;
  if (!XANC("light gray"))
    return 8;
  colors->light_gray = rColor.pixel;
  if (!XANC("dark gray"))
    return 9;
  colors->dark_gray = rColor.pixel;
  if (!XANC("light blue"))
    return 10;
  colors->light_blue = rColor.pixel;
  if (!XANC("light green"))
    return 11;
  colors->light_green = rColor.pixel;
  if (!XANC("light cyan"))
    return 12;
  colors->light_cyan = rColor.pixel;
  if (!XANC("yellow"))
    return 13;
  colors->yellow = rColor.pixel;
  if (!XANC("white"))
    return 14;
  colors->white = rColor.pixel;
  return 0;
}

int CreateBitmaps(Display *prDisplay, Drawable nWnd, Pixmap *prPixmap) {
  char psBitmapBits[17][8] = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
                              {0xff, 0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee},
                              {0xff, 0xbb, 0xff, 0xee, 0xff, 0xbb, 0xff, 0xee},
                              {0xff, 0xaa, 0xff, 0xee, 0xff, 0xaa, 0xff, 0xee},
                              {0xff, 0xaa, 0xff, 0xaa, 0xff, 0xaa, 0xff, 0xaa},
                              {0xff, 0xaa, 0xdd, 0xaa, 0xff, 0xaa, 0xdd, 0xaa},
                              {0xdd, 0xaa, 0xdd, 0xaa, 0xdd, 0xaa, 0xdd, 0xaa},
                              {0x55, 0xaa, 0xdd, 0xaa, 0x55, 0xaa, 0xdd, 0xaa},
                              {0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa},
                              {0xaa, 0x55, 0x22, 0x55, 0xaa, 0x55, 0x22, 0x55},
                              {0x22, 0x55, 0x22, 0x55, 0x22, 0x55, 0x22, 0x55},
                              {0x00, 0x55, 0x22, 0x55, 0x00, 0x55, 0x22, 0x55},
                              {0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55},
                              {0x00, 0x55, 0x00, 0x11, 0x00, 0x55, 0x00, 0x11},
                              {0x00, 0x44, 0x00, 0x11, 0x00, 0x44, 0x00, 0x11},
                              {0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x11},
                              {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
  int i;

  for (i = 0; i <= 16; i++)
    if (!(prPixmap[i] =
              XCreateBitmapFromData(prDisplay, nWnd, psBitmapBits[i], 8, 8)))
      return i + 1;
  return 0;
}

void FreeBitmaps(Display *prDisplay, Pixmap *prPixmap) {
  for (int i = 0; i <= 16; i++)
    XFreePixmap(prDisplay, prPixmap[i]);
}

Display *prDisplay;
int nScreenNum;
GC prGC;
Window nWnd;
Drawable draw;
Pixmap prPixmap[17];

COLORS rColors;
int nWWidth = WDef;
int nWHeight = HDef;

// handlers

bool HandleExpose(void (*DrawWindowContent)(), XEvent Event,
                  XWindowAttributes Attr, int Depth) {
  if (Event.xexpose.count != 0)
    return 0;
  if (XGetWindowAttributes(prDisplay, nWnd, &Attr)) {
    if (nWWidth != Attr.width || nWHeight != Attr.height ||
        Depth != Attr.depth) {
      XFreePixmap(prDisplay, draw);
      nWWidth = Attr.width;
      nWHeight = Attr.height;
      Depth = Attr.depth;
      draw = XCreatePixmap(prDisplay, nWnd, nWWidth, nWHeight, Depth);
      if (!draw)
        return false;
    }
  } else
    return false;
  DrawWindowContent();
  XCopyArea(prDisplay, draw, nWnd, prGC, 0, 0, nWWidth, nWHeight, 0, 0);
  return 0;
}

bool HandleKeyPress(int (*KeyPressFunction)(int), XEvent Event,
                    KeySym nKeySym) // true if quit pressed
{
  XLookupString(&Event.xkey, 0, 0, &nKeySym, 0);
  switch (KeyPressFunction(nKeySym)) {
  case KEY_PRESS_NOTHING:
    break;
  case KEY_PRESS_EXPOSE:
    Event.type = Expose;
    Event.xexpose.count = 0;
    XSendEvent(prDisplay, nWnd, True, 0, &Event);
    break;
  case KEY_PRESS_QUIT:
    XFreePixmap(prDisplay, draw);
    FreeBitmaps(prDisplay, prPixmap);
    XFreeGC(prDisplay, prGC);
    XCloseDisplay(prDisplay);
    return true;
  }
  return false;
}

int DrawWindow(void (*DrawWindowContent)(), int (*KeyPressFunction)(int)) {
  XEvent Event;
  XWindowAttributes Attr;
  KeySym nKeySym;
  int nDepth;

  if ((prDisplay = XOpenDisplay(0)) == 0)
    return 1;

  nScreenNum = DefaultScreen(prDisplay);
  nWnd = XCreateSimpleWindow(prDisplay, RootWindow(prDisplay, nScreenNum), XPos,
                             YPos, WDef, HDef, WBorder,
                             BlackPixel(prDisplay, nScreenNum),
                             WhitePixel(prDisplay, nScreenNum));
  if (SetWindowManagerHints(prDisplay, PClass, nWnd, WMin, HMin, WTitle,
                            WITitle, 0))
    return 2;
  XSelectInput(prDisplay, nWnd, ExposureMask | KeyPressMask);
  XMapWindow(prDisplay, nWnd);

  prGC = XCreateGC(prDisplay, nWnd, 0, 0);
  if (AllocColors(prDisplay, DefaultColormap(prDisplay, nScreenNum), &rColors))
    return 3;
  if (CreateBitmaps(prDisplay, nWnd, prPixmap))
    return 4;
  if (XGetWindowAttributes(prDisplay, nWnd, &Attr)) {
    nWWidth = Attr.width;
    nWHeight = Attr.height;
    nDepth = Attr.depth;
  } else
    return 5;

  draw = XCreatePixmap(prDisplay, nWnd, nWWidth, nWHeight, nDepth);
  if (!draw)
    return 4;

  // message cycle
  while (true) {
    XNextEvent(prDisplay, &Event);
    switch (Event.type) {
    case Expose:
      //	if (!
      HandleExpose(DrawWindowContent, Event, Attr, nDepth);
      //	) return 5; break;
      break;
    case KeyPress:
      if (HandleKeyPress(KeyPressFunction, Event, nKeySym))
        return 0;
      break;
    }
    while (XCheckMaskEvent(prDisplay, KeyPressMask | KeyReleaseMask, &Event))
      ;
  }
  return 0;
}

void WSetColor(unsigned long color) { XSetForeground(prDisplay, prGC, color); }
void WDrawString(const char *string, int x, int y) {
  XDrawString(prDisplay, draw, prGC, x, y, string, strlen(string));
}
void WDrawPoint(int x, int y) { XDrawPoint(prDisplay, draw, prGC, x, y); }
void WDrawPoint(double x, double y) {
  XDrawPoint(prDisplay, draw, prGC, (int)x, (int)y);
}

void WDrawLine(int x_start, int y_start, int x_end, int y_end) {
  XDrawLine(prDisplay, draw, prGC, x_start, y_start, x_end, y_end);
}
void WDrawLine(double x_start, double y_start, double x_end, double y_end) {
  XDrawLine(prDisplay, draw, prGC, (int)x_start, (int)y_start, (int)x_end,
            (int)y_end);
}

void WDrawRectangle(int x_top_left, int y_top_left, int x_bottom_right,
                    int y_bottom_right) {
  XDrawRectangle(prDisplay, draw, prGC, x_top_left, y_top_left,
                 x_bottom_right - x_top_left + 1,
                 y_bottom_right - y_top_left + 1);
}

void WFillRectangle(int x_top_left, int y_top_left, int x_bottom_right,
                    int y_bottom_right) {
  XFillRectangle(prDisplay, draw, prGC, x_top_left, y_top_left,
                 x_bottom_right - x_top_left + 2,
                 y_bottom_right - y_top_left + 2);
}

void WFillTriangle(int x_1, int y_1, int x_2, int y_2, int x_3, int y_3) {
  XPoint points[3] = {{x_1, y_1}, {x_2, y_2}, {x_3, y_3}};
  XFillPolygon(prDisplay, draw, prGC, points, 3, Convex, CoordModeOrigin);
}

void WFillPoligon(XPoint *points, int num) {
  XFillPolygon(prDisplay, draw, prGC, points, num, Convex, CoordModeOrigin);
}

void WSetFillStyle(int Style) { XSetFillStyle(prDisplay, prGC, Style); }
void WSetStipple(unsigned int Index) {
  XSetStipple(prDisplay, prGC, prPixmap[Index]);
}
void WSetTile(unsigned int Index) {
  XSetTile(prDisplay, prGC, prPixmap[Index]);
}
int WGetColor(unsigned int Red, unsigned int Green, unsigned int Blue,
              unsigned long *pColor) {
  XColor rColor;

  rColor.red = Red << 8;
  rColor.green = Green << 8;
  rColor.blue = Blue << 8;
  rColor.flags = DoRed | DoGreen | DoBlue;

  if (XAllocColor(prDisplay, DefaultColormap(prDisplay, nScreenNum), &rColor) ==
      0) /* ошибка */
    return 1;

  *pColor = rColor.pixel;
  return 0;
}

void WSetTitle(const char *s) {
  XTextProperty tProp;
  XStringListToTextProperty((char **)&s, 1, &tProp);
  XSetWMName(prDisplay, nWnd, &tProp);
}
