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
    XSetForegro