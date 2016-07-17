// (C) SEGMENTATION FAULT Softaware, 2003
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

#include "DMVN_GLib.h"

int DMVN_XLib_RGBMode = RGB_565;
static int AttrList_Double_Buf[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_DEPTH_SIZE, 32, None };
static int AttrList_Single_Buf[] = {GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_DEPTH_SIZE, 32, None };


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
    try
    {
	printf("\nLoading GL X-Window library DMVN_GLib...\n");
	printf("(C) SEGMENTATION FAULT Software\n");
	printf("Build on Saturday, 24.05.2003\n\n");
	Initialized = false;
	ErrorPresent = false;
	printf("INIT: STARTED\n");
	printf("INIT: Opening display...");
	Disp = XOpenDisplay(NULL);
	if (!Disp) throw 1;
	printf("OK\n");
	Scr = DefaultScreen(Disp);
	printf("INIT: Creating window...");
	Win = XCreateSimpleWindow(Disp, DefaultRootWindow(Disp), X, Y, Width, Height, 0, BlackPixel(Disp, Scr), WhitePixel(Disp, Scr));
	printf("OK\n");
	printf("INIT: Setting window attributes...");
	XSetStandardProperties(Disp, Win, Title, NULL, None, NULL, 0, NULL);
	printf("OK\n");
	printf("INIT: Selecting message types: Exposure, ButtonPress, KeyPress...");
	XSelectInput(Disp, Win, ExposureMask | ButtonPressMask | KeyPressMask);	
	printf("OK\n");
	printf("INIT: Creating graphics context...");
	GContext = XCreateGC(Disp, Win, 0, 0);	
	printf("OK\n");
	printf("INIT: Mapping window...");
	XMapRaised(Disp, Win);
	printf("OK\n");
	DMVN_XLib_RGBMode = RGBMode;
	printf("INIT: COMPLETED\n");
	XInitialized = true;
	
	// INIT OpenGL
	printf("INIT_GL: STARTED\n");
	printf("INIT_GL: Selecting GL Visual...\n");
	GLVisual = glXChooseVisual(Disp, Scr, AttrList_Double_Buf);
	if (GLVisual)
	{
	    GLSwap = true; // Double buffering
	    printf("OK, using Double buffering.\n");
	}
	else
	{
	    GLVisual = glXChooseVisual(Disp, Scr, AttrList_Single_Buf);
	    if (!GLVisual) throw 2;
	    printf("OK, Using Single buffering.\n");
	    GLSwap = false;
	}
	memset(GLContext, 0, sizeof(GLXContext));
	printf("INIT_GL: Creating OpenGL Context...");
	GLContext = glXCreateContext(Disp, GLVisual, 0, GL_TRUE);
	if (!GLContext)
	{
	    GLContextCreated = false;
	    throw 3;
	}
	GLContextCreated = true;
	printf("OK\n");
			
    }
    catch (int EN)
    {
	printf("FAILED\n");
	ErrorPresent = true;
    }
}

TXWin::~TXWin(void)
{
    printf("SHUTDOWN_GL: STARTED\n");
    if (GLContextCreated)
    {
	printf("SHUTDOWN_GL: Destroying GLContext...");
	glXDestroyContext(Disp, GLContext);
    }
    printf("OK\n");
    printf("SHUTDOWN_GL: COMPLETED\n");
    printf("SHUTDOWN: STARTED\n");
//    if (!Initialized) return;
    printf("SHUTDOWN: Freeing GContext...");
    XFreeGC(Disp, GContext);
    printf("OK\n");
    printf("SHUTDOWN: Destroying window...");
    XDestroyWindow(Disp, Win);
    printf("OK\n");
    printf("SHUTDOWN: Closing Display...");
    XCloseDisplay(Disp);    
    printf("OK\n");
    printf("SHUTDOWN: COMPLETED\n");    
}

void TXWin::GetEvent(int * EventType, TKeyInfo * KeyInfo, TMouseInfo * MouseInfo)
{
    XEvent Event;
    XNextEvent(Disp, &Event);
    switch (Event.type)
    {
	case Expose:
	    *EventType = ET_EXPOSE;
	    break;
	case KeyPress:	    
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
}

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

void TXWin::FillRect(int X, int Y, int Width, int Height)
{
    XSetForeground(Disp, GContext, RGB(0, 0, 0));    
    XFillRectangle(Disp, Win, GContext, X, Y, Width, Height);
}
