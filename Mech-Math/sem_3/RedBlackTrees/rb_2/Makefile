 &Event)) return false;
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
		case Button2