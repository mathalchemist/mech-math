#include <stdio.h>
#include <conio.h>

#include <list>

using namespace std;

class TCell {
public:
  double v;
  int i;
  TCell() {
    v = 0.0;
    i = 0;
  }
  TCell(const TCell &c) {
    i = c.i;
    v = c.v;
  }
};

typedef list<TCell> TList;
typedef list<TCell>::iterator lit;

class sm {
public:
  int w;
  int h;

  TList *r;
  sm(int iw, int ih);
  ~sm();
  void get_row(int i, double *row);
  void get_col(int i, double *col);

  void sm::ex_rows(int y1, int y2);
  void sm::ex_cols(int x1, int x2);

  void sm::lin_comb(int y1, int y2, double lambda);

  double ge(int x, int y);
  void se(int x, int y, double v);
};

sm::sm(int iw, int ih) : w(iw), h(ih) { r = new TList[h]; }

sm::~sm() { delete[] r; }

double sm::ge(int x, int y) {
  if (y >= h)
    return 0;
  if (x >= w)
    return 0;
  for (lit it = r[y].begin(); it != r[y].end(); it++)
    if (it->i == x)
      return it->v;
  return 0.0;
}

void sm::se(int x, int y, double v) {
  if (y >= h)
    return;
  if (x >= w)
    return;
  for (lit it = r[y].begin(); it != r[y].end(); it++)
    if (it->i == x) {
      it->v = v;
      return;
    }

  TCell c;
  c.i = x;
  c.v = v;
  r[y].push_back(c);
}

void sm::get_row(int i, double *row) {
  if (i >= h)
    return;
  for (int j = 0; j < w; j++)
    row[j] = 0.0;
  for (lit it = r[i].begin(); it != r[i].end(); it++)
    row[it->i] = it->v;
}

void sm::get_col(int i, double *col) {
  if (i >= h)
    return;
  for (int j = 0; j < w; j++)
    col[j] = 0.0;
  for (int j = 0; j < h; j++) {
    bool found = false;
    for (lit it = r[j].begin(); it != r[j].end(); it++)
      if (it->i == i) {
        col[i] = it->v;
        found = true;
        break;
      }
    if (!found)
      col[j] = 0.0;
  }
}

void sm::ex_rows(int y1, int y2) {
  for (int i = 0; i < w; i++) {
    double v1 = ge(i, y1);
    double v2 = ge(i, y2);
    se(i, y1, v2);
    se(i, y2, v1);
  }
}

void sm::ex_cols(int x1, int x2) {
  for (int i = 0; i < h; i++) {
    double v1 = ge(x1, i);
    double v2 = ge(x2, i);
    se(x1, i, v2);
    se(x2, i, v1);
  }
}

void sm::lin_comb(int y1, int y2, double lambda) {
  for (int i = 0; i < w; i++)
    se(i, y1, ge(i, y1) + lambda * ge(i, y2));
}

int main() {
  sm m(10, 10);
  m.se(1, 1, 1.0);
  m.se(1, 1, 2.0);
  printf("%1.5lf\n", m.ge(1, 1));
  getch();
  double a[10];
  m.get_row(1, a);
  for (int i = 0; i < 10; i++)
    printf("%1.5lf ", a[i]);
  printf("\n");
  getch();
  m.get_col(1, a);
  for (int i = 0; i < 10; i++)
    printf("%1.5lf ", a[i]);
  printf("\n");

  for (int i = 0; i < 10; i++)
    for (int j = 0; j < 10; j++)
      m.se(i, j, 1.0 / double(i * j + 1));
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++)
      printf("%1.3lf ", m.ge(i, j));
    printf("\n");
  }
  getch();
  return 0;
}
