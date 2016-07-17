// Slice of the cube
// (C) SEGMENTATION FAULT Software inc., 2003

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "DMVN_XLib.h"
#include "3DMaths.h"

#define R_ANG 0.01
#define R_X 150
#define R_Y 150
#define R_SCALE ((R_X + R_Y) / 8)
#define MAX_FACES 6
#define Cz -50.5

class TFace {
public:
  TVector P[4];
  TVector Normal;
};

class TSegment {
public:
  TVector P[2];
  TSegment(void) {}
  TSegment(TVector P1, TVector P2) {
    P[0] = P1;
    P[1] = P2;
  }
};

class TSP // Screen point
    {
public:
  int x, y;
};

// Window class variable
TXWin *XWin;

double AngleV = 1.57;
double AngleH = 0;

TFace Faces[6];
TSegment Segments[12];
int NumPoints = 0;

TVector SP[6]; // there can't be more than 6 intersection points!

// Vector to turn around
TVector DirX(1, 0, 0);
TVector DirY(0, 1, 0);
TVector DirZ(0, 0, 1);

// PLANE PARAMETERS ===========================
TVector P(-0.2, 0.2, 0.3); // point on the plane
TVector N(0.6, 1.0, 1.1);  // normal to the plane

// prototypes
void ExposeHandler(void);
void KeyboardHandler(TKeyInfo KInfo);
void MouseHandler(TMouseInfo MInfo);

void InitFaces(void);
bool CalcSlice(void);
void RotateFaces(TVector D, double Angle);
bool PointInSlice(int x, int y, TSP *P);
void ProjectFaces(void);

// =============
// Main function
// =============
int main() {
  InitFaces();
  if (!CalcSlice()) {
    printf("Dimension of slice is less than 2. Couldn't display slice.\n");
    return 0;
  }
  // Initializing of XLib & creation of window
  XWin = new TXWin("Cube slice", 10, 10, R_X, R_Y, RGB_565);
  //    if (XWin->Error()) delete XWin;

  while (XWin) {
    int EType;

    TMouseInfo MInfo;
    TKeyInfo KInfo;
    XWin->GetEvent(&EType, &KInfo, &MInfo);

    switch (EType) {
    case ET_EXPOSE:
      ExposeHandler();
      break;
    case ET_KEYPRESS:
      KeyboardHandler(KInfo);
      break;
    case ET_BUTTONPRESS:
      MouseHandler(MInfo);
      break;
    default:
      printf("Unknown event occured.\n");
    }
  }
}

// Application event handlers
// expose handler -- window must be repainted.
void ExposeHandler(void) {
  //    ProjectFaces();
}

void KeyboardHandler(TKeyInfo KInfo) {
  switch (KInfo.KeyCode) {
  case 27:
    delete XWin;
    XWin = NULL;
    break;
  case 'z':
    RotateFaces(DirX, R_ANG);
    break;
  case 'x':
    RotateFaces(DirY, R_ANG);
    break;
  case 'c':
    RotateFaces(DirZ, R_ANG);
    break;
  case 'a':
    RotateFaces(DirX, -R_ANG);
    break;
  case 's':
    RotateFaces(DirY, -R_ANG);
    break;
  case 'd':
    RotateFaces(DirZ, -R_ANG);
    break;
  default:
    break;
  }
  ProjectFaces();
}

void MouseHandler(TMouseInfo MInfo) {
  switch (MInfo.Button) {
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

void InitFaces() {
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

  Segments[0] = TSegment(TVector(1, 1, 1), TVector(1, 1, -1));
  Segments[1] = TSegment(TVector(1, 1, -1), TVector(1, -1, -1));
  Segments[2] = TSegment(TVector(1, -1, -1), TVector(1, -1, 1));
  Segments[3] = TSegment(TVector(1, -1, 1), TVector(1, 1, 1));
  Segments[4] = TSegment(TVector(1, 1, 1), TVector(-1, 1, 1));
  Segments[5] = TSegment(TVector(-1, 1, 1), TVector(-1, -1, 1));
  Segments[6] = TSegment(TVector(-1, -1, 1), TVector(1, -1, 1));
  Segments[7] = TSegment(TVector(-1, 1, 1), TVector(-1, 1, -1));
  Segments[8] = TSegment(TVector(-1, 1, -1), TVector(1, 1, -1));
  Segments[9] = TSegment(TVector(-1, 1, -1), TVector(-1, -1, -1));
  Segments[10] = TSegment(TVector(-1, -1, -1), TVector(1, -1, -1));
  Segments[11] = TSegment(TVector(-1, -1, 1), TVector(-1, -1, 1));
}

double Eq(TVector V) // equation of the plane with normal N & point P0
{
  return ((V - P) | N);
}

bool CalcSlice(void) {
  for (int I = 0; I < 12; I++) {
    TVector A = Segments[I].P[0]; // startpoint of line
    TVector B = Segments[I].P[1]; // endpoint of line
    TVector V = B - A;            // directional vector of this line
    double D = V | N;
    if (D == 0)
      continue; // skip lines parallel to plane
    double t = (N | (P - A)) / D;
    if (Eq(A) * Eq(B) > 0)
      continue; // skip points that doesn't belong to segment.
    if (NumPoints > 5)
      return false; // Oh, shit, SEGMENTATION FAULT EXPECTED!!!
    SP[NumPoints] = A + V * t;
    NumPoints++;
  }
  if (NumPoints < 3)
    return false;
  return true;
}

void RotateFaces(TVector D, double Angle) {
  for (int I = 0; I < MAX_FACES; I++) {
    for (int J = 0; J < 4; J++)
      Faces[I].P[J].Rotate(&D, Angle);
    Faces[I].Normal.Rotate(&D, Angle);
  }
  for (int I = 0; I < NumPoints; I++)
    SP[I].Rotate(&D, Angle);
}

int LineEq(TSP P1, TSP P2, int x, int y) {
  return (x - P1.x) * (P2.y - P1.y) - (y - P1.y) * (P2.x - P1.x);
}

int LineEq(TSP P1, TSP P2, TSP P) {
  return (P.x - P1.x) * (P2.y - P1.y) - (P.y - P1.y) * (P2.x - P1.x);
}

bool PointInSlice(int x, int y, TSP *P) {
  for (int I = 0; I < NumPoints; I++)
    for (int J = 1; J < NumPoints; J++)
      for (int K = 2; K < NumPoints; K++)
        if (I != J && I != K && J != K)
          if (LineEq(P[I], P[J], x, y) * LineEq(P[I], P[J], P[K]) >= 0 &&
              LineEq(P[J], P[K], x, y) * LineEq(P[J], P[K], P[I]) >= 0 &&
              LineEq(P[K], P[I], x, y) * LineEq(P[K], P[I], P[J]) >= 0)
            return true;
  return false;
}

void ProjectFaces(void) {
  TSP CSP[24];
  TSP SSP[6];
  if (!XWin)
    return;

  // 1.1) project slice points
  for (int I = 0; I < NumPoints; I++) {
    SSP[I].x =
        int(R_SCALE * SP[I].Coords[0] * Cz / ((SP[I].Coords[2] + 3) - Cz));
    SSP[I].y =
        int(R_SCALE * SP[I].Coords[1] * Cz / ((SP[I].Coords[2] + 3) - Cz));
  }
  // 1.2) project cube wireframe points
  for (int I = 0; I < MAX_FACES; I++)
    for (int J = 0; J < 4; J++) {
      CSP[I * 4 + J].x = int(R_SCALE * Faces[I].P[J].Coords[0] * Cz /
                             ((Faces[I].P[J].Coords[2] + 3) - Cz));
      CSP[I * 4 + J].y = int(R_SCALE * Faces[I].P[J].Coords[1] * Cz /
                             ((Faces[I].P[J].Coords[2] + 3) - Cz));
    }
  // Clear next frame & repaint... IMMEDEATELY!!!
  XWin->FillRect(0, 0, R_X, R_Y);
  // 2.1) display "invisible" polygons (dark)
  for (int I = 0; I < MAX_FACES; I++)
    if (Faces[I].Normal.Coords[2] >= 0)
      for (int J = 0; J < 4; J++)
        XWin->DrawLine(R_X / 2 + CSP[I * 4 + J].x, R_Y / 2 - CSP[I * 4 + J].y,
                       R_X / 2 + CSP[I * 4 + (J + 1) % 4].x,
                       R_Y / 2 - CSP[I * 4 + (J + 1) % 4].y, RGB(84, 84, 84));

  // 2.2) display slice - texturing with texture 1x1 pixel
  // 2.2.1) get bounding rectangle for slice points
  int Left = SSP[0].x;
  int Right = SSP[0].x;
  int Bottom = SSP[0].y;
  int Top = SSP[0].y;
  for (int I = 1; I < NumPoints; I++) {
    if (SSP[I].x < Left)
      Left = SSP[I].x;
    if (SSP[I].x > Right)
      Right = SSP[I].x;
    if (SSP[I].y < Bottom)
      Bottom = SSP[I].y;
    if (SSP[I].y > Top)
      Top = SSP[I].y;
  }
  // 2.2.2) check all the points in rectangle and paint matching points.
  for (int x = Left; x <= Right; x++)
    for (int y = Bottom; y <= Top; y++)
      if (PointInSlice(x, y, SSP))
        XWin->DrawPoint(R_X / 2 + x, R_Y / 2 - y, RGB(0, 0, 255));

  // 2.3) display "visible" polygons (bright)
    for (int I = 0; I < MAX_FACES; I++)
	if (Faces[I].Normal.Coords[2] < 0)
    	    for (int J = 0; J < 4; J++) XWin->DrawLine(R_X/2 + CSP[I * 4 + J].x, R_Y/2 - CSP[I * 4 + J].y, R_X/2 + CSP[I * 4 + (J + 1) % 4].x, R_Y/2 - CSP[I * 4 + (J + 1) % 4].y, RGB(255, 255, 255));
}
