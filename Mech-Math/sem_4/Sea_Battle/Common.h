// =====================================
// Common defs
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================

// Sea battle common decls
TXWin * XWin;

bool HeIsReady = false;
bool EndedByOtherSide = false;
bool EarlyEnd = false;

const int XWW = 500;
const int XWH = 270;
enum CELLSTATE { CS_EMPTY, CS_SHIP, CS_MISS, CS_HIT };
enum GAMESTATE { GS_INPUT, GS_READY, GS_GAME, GS_END };

int GameState = GS_INPUT;

const int SIZE = 16;
const int MAX = 256;
const int D=7;

int MyField[10][10];
int HisField[10][10];

int MyHits = 0;
int HisHits = 0;

bool Victory = false;

int SendBuf[MAX];
int RecvBuf[MAX];

int CursorX = 0;
int CursorY = 0;

TKeyInfo KInfo;
TMouseInfo MInfo;

void SigHandler(int);
void NetworkHandler(int Socket);
int KeyHandler(int Socket);
void ExposeHandler();
bool WaitMessage(int Socket, int * Buffer);
bool Working = true;
