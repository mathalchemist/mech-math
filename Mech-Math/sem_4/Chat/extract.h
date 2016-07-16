enum MESSAGETYPE
{
    MT_UNKNOWN,
    MT_MESSAGE,
    MT_KILL_TS,
    MT_KILL_FS,
    MT_QUIT_TS,
    MT_QUIT_FS,
    MT_BROADCAST
};

int ExtractMsg(string * Buffer, string * UserName, string * Message)
{
    string Buf = *Buffer;
    string UN;
    string Msg;
    int i = 1;
    char P = Buf[0];
    // M | Q | K | B UserName # Msg
    int L = Buf.length(); 
    
    if (P == 'M' ||
        P == 'Q' ||
	P == 'q' ||
	P == 'K' ||
	P == 'k' ||
	P == 'B')
    {
	while (i < L && Buf[i] != '#') { UN += Buf[i]; i++; }
	if (i == L) return MT_UNKNOWN;
	i++;
	while (i < L) { Msg += Buf[i]; i++; }
	if (UN == "") UN = "[unnamed]";
	if (Msg == "") Msg = "[empty message]";
    }
    *UserName = UN;
    *Message = Msg;
    if (P == 'M') return MT_MESSAGE;
    if (P == 'Q') return MT_QUIT_TS;
    if (P == 'q') return MT_QUIT_FS;
    if (P == 'K') return MT_KILL_TS;
    if (P == 'k') return MT_KILL_FS;
    if (P == 'B') return MT_BROADCAST;
    return MT_UNKNOWN;
}
