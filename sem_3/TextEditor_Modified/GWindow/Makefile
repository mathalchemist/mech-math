 }
}

TXWin::TXWin(char * Title, int X, int Y, int Width, int Height, int RGBMode)
{
    Disp = XOpenDisplay(NULL);
    if (!Disp) throw Ex("Cannot open display");
    Scr = DefaultScreen(Disp);
    Win = XCreateSimpleWindow(Disp, DefaultRootWindow(Disp), X, Y, Width, Height, 0, BlackPixel(Disp, Scr), WhitePixel(Disp, Scr));
    XSetStandardProperties(Disp, Win, Title, NULL, None, NULL