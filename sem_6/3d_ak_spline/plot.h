#ifndef PLOT_H
#define PLOT_H

#include <X11/keysym.h>

typedef struct
{
  unsigned long black;
  unsigned long blue;
  unsigned long green;
  unsigned long cyan;
  unsigned long red;
  unsigned long magenta;
  unsigned long brown;
  unsigned long light_gray;
  unsigned long dark_gray;
  unsigned long light_blue;
  unsigned long light_green;
  unsigned long light_cyan;
  unsigned long yellow;
  unsigned long white;
}
COLORS;

#define BLACK           (rColors.black)
#define BLUE            (rColors.blue)
#define GREEN           (rColors.green)
#define CYAN            (rColors.cyan)
#define RED             (rColors.red)
#define MAGENTA         (rColors.magenta)
#define BROWN           (rColors.brown)
#define LIGHTGRAY       (rColors.light_gray)
#define DARKGRAY        (rColors.dark_gray)
#define LIGHTBLUE       (rColors.light_blue)
#define LIGHTGREEN      (rColors.light_green)
#define LIGHTCYAN       (rColors.light_cyan)
#define YELLOW          (rColors.yellow)
#define WHITE           (rColors.white)

extern COLORS rColors;	
extern int nWWidth;
extern int nWHeight;

#define KEY_PRESS_NOTHING	0
#define KEY_PRESS_EXPOSE	1
#define KEY_PRESS_QUIT		(-1)

int DrawWindow(void (*DrawWindowContent)(), int (*KeyPressFunction)(int));

void WSetColor(unsigned long color);
void WDrawString(const char *string, int x, int y);
void WDrawPoint(int x, int y);
void WDrawPoint(double x, double y);
void WDrawLine(int x_start, int y_start, int x_end, int y_end);
void WDrawLine(double x_start, double y_start, double x_end, double y_end);
void WDrawRectangle (int x_top_left, int y_top_left, int x_bottom_right, int y_bottom_right);
void WFillRectangle (int x_top_left, int y_top_left, int x_bottom_right, int y_bottom_right);
void WFillTriangle (int x_1, int y_1, int x_2, int y_2, int x_3, int y_3);

//Styles: FillSolid, FillTiled, FillOpaqueStippled, FillStippled
void WSetFillStyle (int Style);
void WSetStipple (unsigned int Index);
void WSetTile (unsigned int Index);
int WGetColor (unsigned int Red, unsigned int Green, unsigned int Blue, unsigned long *pColor);
void WSetTitle (const char *s);

#endif
