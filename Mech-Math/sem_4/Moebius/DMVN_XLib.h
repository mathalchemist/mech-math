#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>

#include <GL/glx.h> 
#include <GL/gl.h> 

#ifndef __DMVN_XLIB_H_INCLUDED__
#define __DMVN_XLIB_H_INCLUDED__

typedef unsigned long DWORD;

DWORD RGB(int R, int G, int B);

enum RGBModes { RGB_555, RGB_565, RGB_888 };
enum EventTypes { ET_UNKNOWN, ET_EXPOSE, ET_KEYPRESS, ET_BUTTONPRESS };
enum MouseButtons { MB_UNKNOWN, MB_LEFT, MB_MIDDLE, MB_RIGHT, MB_MWHEELUP, MB_MWHEELDOWN };

class TMouseInfo { public: int X; int Y; int Button; };
class TKeyInfo { public: char KeyCode; DWORD KeyData; unsigned int KeyState; };


static int AttrList_Double_Buf[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_DEPTH_SIZE, 32, None };
static int AttrList_Single_Buf[] = {GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_DEPTH_SIZE, 32, None };

class TXWin
{
    // openGL
    bool GLSwap;
    bool GLContextCreated;
    XVisualInfo * GLVisual;
    GLXContext GLContext;

    Display * Disp;
    int Scr;
    Window Win;
    GC GContext;
    bool Initialized;
    bool ErrorPresent;    
public:
    class Ex
    {
    public:
	const char * Msg;
	Ex(): Msg("Unknown error") {}
	Ex(const char * msg): Msg(msg) {}	
    };
    TXWin(char * Title, int X, int Y, int Width, int Height, int RGBMode);
    ~TXWin();
    bool GetEvent(int * EventType, TKeyInfo * KeyInfo, TMouseInfo * MouseInfo);
    void SetColor(DWORD Color);
    void DrawPoint(int X, int Y, DWORD Color);
    void DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color);
    void DrawRect(int X1, int Y1, int X2, int Y2, DWORD Color);
    void FillRect(int X, int Y, int Width, int Height);
    void Write(int X, int Y, char * String);
};

#endif
