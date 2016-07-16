in, GContext, X1, Y1, X2, Y1);
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
    XSetForeground