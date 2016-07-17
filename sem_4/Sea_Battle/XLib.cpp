// =====================================
// Absolutely Free Graphic X-Library
// More usable than 80ri5enk0's one ;-)
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

#include "XLib.h"

int DMVN_XLib_RGBMode = RGB_565;

DWORD RGB(int R, int G, int B)
{
    switch (DMVN_XLib_RGBMode)
    {
	case RGB_555: return ((R >> 3) << 10) + ((G >> 3) << 5) + (B >> 3);
	case RGB_565: return ((R >> 3) << 11) + ((G >> 2) << 5) + (B >> 3);
	case RGB_888: return (R << 16) + (G << 8) + B;
	default: return 0;
    }
}

TXWin::TXWin(char * Title, int X, int Y, int Width, int Height, int RGBMode)
{
    Disp = XOpenDisplay(NULL);
    if (!Disp) throw Ex("Cannot open display");
    Scr = DefaultScreen(Disp);
    Win = XCreateSimpleWindow(Disp, DefaultRootWindow(Disp), X, Y, Width, Height, 0, BlackPixel(Disp, Scr), WhitePixel(Disp, Scr));
    XSetStandardProperties(Disp, Win, Title, NULL, None, NULL, 0, NULL);
    XSelectInput(Disp, Win, ExposureMask | ButtonPressMask | KeyPressMask);	
    GContext = XCreateGC(Disp, Win, 0, 0);	
    XMapRaised(Disp, Win);
    DMVN_XLib_RGBMode = RGBMode;
}

TXWin::~TXWin(void)
{
    XFreeGC(Disp, GContext);
    XDestroyWindow(Disp, Win);
    XCloseDisplay(Disp);    
}

bool TXWin::GetEvent(int * EventType, TKeyInfo * KeyInfo, TMouseInfo * MouseInfo)
{
    XEvent Event;
    if (!XCheckWindowEvent(Disp, Win, ExposureMask | ButtonPressMask | KeyPressMask, &Event)) return false;
    switch (Event.type)
    {
	case Expose:
	    *EventType = ET_EXPOSE;
	    break;
	case KeyPress:	    
	    KeyInfo->KeyState = Event.xkey.state;
	    if (XLookupString(&Event.xkey, &KeyInfo->KeyCode, 1, &KeyInfo->KeyData, 0)==1) *EventType = ET_KEYPRESS;
	    break;
	case ButtonPress:
	    MouseInfo->X = Event.xbutton.x;
	    MouseInfo->Y = Event.xbutton.y;
	    switch (Event.xbutton.button)
	    {
		case Button1: MouseInfo->Button = MB_LEFT; break;
		case Button2: MouseInfo->Button = MB_MIDDLE; break;
		case Button3: MouseInfo->Button = MB_RIGHT; break;
		case Button4: MouseInfo->Button = MB_MWHEELUP; break;
		case Button5: MouseInfo->Button = MB_MWHEELDOWN; break;
		default: MouseInfo->Button = MB_UNKNOWN;
	    }
	    *EventType = ET_BUTTONPRESS;
	    break;
	default:
	    *EventType = ET_UNKNOWN;
	    break;
    }
    return true;
}

void TXWin::SetColor(DWORD Color) { XSetForeground(Disp, GContext, Color); }

void TXWin::DrawPoint(int X, int Y, DWORD Color)
{
    XSetForeground(Disp, GContext, Color);
    XDrawPoint(Disp, Win, GContext, X, Y);
}

void TXWin::DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color)
{
    XSetForeground(Disp, GContext, Color);
    XDrawLine(Disp, Win, GContext, X1, Y1, X2, Y2);
}

void TXWin::DrawRect(int X1, int Y1, int X2, int Y2, DWORD Color)
{
    XSetForeground(Disp, GContext, Color);
    XDrawLine(Disp, Win, GContext, X1, Y1, X1, Y2);
    XDrawLine(Disp, Win, GContext, X1, Y1, X2, Y1);
    XDrawLine(Disp, Win, GContext, X2, Y1, X2, Y2);
    XDrawLine(Disp, Win, GContext, X1, Y2, X2, Y2);
}

void TXWin::FillRect(int X, int Y, int Width, int Height)
{
    XSetForeground(Disp, GContext, RGB(0, 0, 0));    
    XFillRectangle(Disp, Win, GContext, X, Y, Width, Height);
}

void TXWin::Write(int X, int Y, char * String)
{
    XSetForeground(Disp, GContext, RGB(255, 255, 255));    
    XDrawString(Disp, Win, GContext, X, Y, String, strlen(String));
}

