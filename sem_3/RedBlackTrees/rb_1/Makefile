, 0, NULL);
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
    if (!XCheckWindowEvent(Disp, Win, ExposureMask | ButtonPressMask | KeyPressMask,