und(Disp, GContext, Color);
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
    XDrawStri