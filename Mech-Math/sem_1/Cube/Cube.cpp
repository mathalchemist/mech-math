// Hidden surface removing sample - flipping cube
// (C) DMVN Corp, 2003

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "DMVN_XLib.h"
#include "3DMaths.h"

#define R_SENS 0.05
#define R_X 300
#define R_Y 300
#define R_SCALE 70
#define MAX_FACES 6
#define Cz -100.5

void ExposeHandler(void);
void KeyboardHandler(TKeyInfo KInfo);
void MouseHandler(TMouseInfo MInfo);

TVector CalcDirVector(void);
void InitFaces(void);
void RotateFaces(TVector D);
void ProjectFaces();

class TFace
{
public:
   TVector P[4];
   TVector Normal;
};

class TSP // Screen point
{
public:
   int x, y;
};

// Window class variable
TXWin * XWin;

double AngleV = 0.78;
double AngleH = 0.78;

TFace Faces[6];

// Vector to turn around
TVector Dir(0,0,1);

// =============
// Main function
// =============
int main()
{
   InitFaces();
   Dir = CalcDirVector();
    // Initializign of XLib & creation of window
    XWin = new TXWin("Hidden surface removing", 10, 10, R_X, R_Y, RGB_565);

    while (XWin)
    {		
	int EType;

	TMouseInfo MInfo;
	TKeyInfo KInfo;
	XWin->GetEvent(&EType, &KInfo, &MInfo);
	
	switch (EType)
	{
	    case ET_EXPOSE: ExposeHandler(); break;
	    case ET_KEYPRESS: KeyboardHandler(KInfo); break;
	    case ET_BUTTONPRESS: MouseHandler(MInfo); break;
	    default: printf("Unknown event occured.\n");
	}
    }
}

// Application event handlers
// expose handler -- window must be repainted.
void ExposeHandler(void)
{
    ProjectFaces();
}

void KeyboardHandler(TKeyInfo KInfo)
{
    switch (KInfo.KeyCode)
    {
	case 27: delete XWin; XWin = NULL; break;
        case ' ': RotateFaces(Dir); break;
	case 'a': AngleH += R_SENS; break;
        case 'd': AngleH -= R_SENS; break;
        case 'w': AngleV += R_SENS; break;
        case 's': AngleV -= R_SENS; break;
	default: break;
    }
    Dir = CalcDirVector();
    ProjectFaces();
}

void MouseHandler(TMouseInfo MInfo)
{
    switch (MInfo.Button)
    {
	case MB_LEFT:
	    break;
	case MB_RIGHT:
	    break;
	case MB_MWHEELUP:
	    break;
	case MB_MWHEELDOWN:
	    break;
    }
}

// returns normalized rotated vector
TVector CalcDirVector()
{
   TVector V(0, 0, 1);
   V.RotateY(AngleH);
   TVector HorzNormal(-V.Coords[2], 0, V.Coords[0]);
   V.Rotate(&HorzNormal, AngleV);
   V.Normalize();
   return V;
}

void InitFaces()
{
   Faces[0].P[0] = TVector(1, 1, 1);
   Faces[0].P[1] = TVector(1, -1, 1);
   Faces[0].P[2] = TVector(-1, -1, 1);
   Faces[0].P[3] = TVector(-1, 1, 1);
   Faces[0].Normal = TVector(0, 0, 1);
   Faces[1].P[0] = TVector(1, 1, 1);
   Faces[1].P[1] = TVector(1, 1, -1);
   Faces[1].P[2] = TVector(1, -1, -1);
   Faces[1].P[3] = TVector(1, -1, 1);
   Faces[1].Normal = TVector(1, 0, 0);
   Faces[2].P[0] = TVector(-1, -1, 1);
   Faces[2].P[1] = TVector(-1, -1, -1);
   Faces[2].P[2] = TVector(1, -1, -1);
   Faces[2].P[3] = TVector(1, -1, 1);
   Faces[2].Normal = TVector(0, -1, 0);
   Faces[3].P[0] = TVector(-1, 1, 1);
   Faces[3].P[1] = TVector(1, 1, 1);
   Faces[3].P[2] = TVector(1, 1, -1);
   Faces[3].P[3] = TVector(-1, 1, -1);
   Faces[3].Normal = TVector(0, 1, 0);
   Faces[4].P[0] = TVector(-1, 1, 1);
   Faces[4].P[1] = TVector(-1, 1, -1);
   Faces[4].P[2] = TVector(-1, -1, -1);
   Faces[4].P[3] = TVector(-1, -1, 1);
   Faces[4].Normal = TVector(-1, 0, 0);
   Faces[5].P[0] = TVector(-1, 1, -1);
   Faces[5].P[1] = TVector(1, 1, -1);
   Faces[5].P[2] = TVector(1, -1, -1);
   Faces[5].P[3] = TVector(-1, -1, -1);
   Faces[5].Normal = TVector(0, 0, -1);
}

void RotateFaces(TVector D)
{
   for (int I = 0; I < MAX_FACES; I++)
   {
      for (int J = 0; J < 4; J++) Faces[I].P[J].Rotate(&D, 0.01);
      Faces[I].Normal.Rotate(&D, 0.01);
   }
}

void ProjectFaces()
{
    TSP SP[4];
    TSP DSP;
    if (!XWin) return;
    XWin->FillRect(0, 0, R_X, R_Y);
    for (int I = 0; I < MAX_FACES; I++)
	if (Faces[I].Normal.Coords[2] < 0) // hidden surface removing
	{	
	    for (int J = 0; J < 4; J++)
    	    {	
        	SP[J].x = int(R_SCALE * Faces[I].P[J].Coords[0] * Cz / ((Faces[I].P[J].Coords[2] + 3) - Cz));
        	SP[J].y = int(R_SCALE * Faces[I].P[J].Coords[1] * Cz / ((Faces[I].P[J].Coords[2] + 3) - Cz));
    	    }
    	    for (int J = 0; J < 4; J++) XWin->DrawLine(R_X/2 + SP[J].x, R_Y/2 - SP[J].y, R_X/2 + SP[(J + 1) % 4].x, R_Y/2 - SP[(J + 1) % 4].y, RGB(255, 255, 255));
	}
    // axis
    DSP.x = int(R_SCALE * Dir.Coords[0] * Cz / ((Dir.Coords[2] + 3) - Cz));
    DSP.y = int(R_SCALE * Dir.Coords[1] * Cz / ((Dir.Coords[2] + 3) - Cz));
    XWin->DrawLine(R_X/2, R_Y/2, R_X/2 + DSP.x, R_Y/2 - DSP.y, RGB(255, 0, 0));
}
