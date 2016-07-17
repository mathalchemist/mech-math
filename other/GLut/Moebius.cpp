// Moebius surface
// (C) rdmr, 8 Mar 2004

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glut.h>

#include "3DMaths.h"

const float DeAngle = 0.2;

class Seg { public: TVector p1, p2, n; };

const int Max = 200;
const double dMax = double(Max);
const double Pi = 3.14159265;

const double R = 5;
const double W = 1;

Seg Segs[Max];

int T0 = 0;
int Frames = 0;
double phase=0;

GLfloat view_rotx = 20.0, view_roty = 30.0, view_rotz = 0.0;
float angle = 0.0;

void draw()
{
    phase += 0.01;

    GLfloat Color[4] = {0, 0, 0.0, 1.0};
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
//    glRotatef(view_rotx, 1.0, 0.0, 0.0);
//    glRotatef(view_roty, 0.0, 1.0, 0.0);
//    glRotatef(view_rotz, 0.0, 0.0, 1.0);
	
//    glTranslatef(-3.0, -2.0, 0.0);


    glRotatef(angle, 0.1, 1.0, 0.5);

    //  glShadeModel(GL_FLAT);
    
    glShadeModel(GL_SMOOTH);
    
    glBegin(GL_QUAD_STRIP);
    double fi = 2 * Pi / dMax;
    for (int i = 0; i < Max; i++)
    {
	double fi1 = double(i) * fi;
	Color[0] = fabs(cos(2 * fi1 + phase));
	Color[1] = fabs(sin(2 * fi1 - phase));
	Color[1] = fabs(sin(fi1 - 2 *  phase));
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Color);
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, Color);
	glNormal3f(Segs[i].n.Coords[0], Segs[i].n.Coords[1], Segs[i].n.Coords[2]);
	glVertex3f(Segs[i].p1.Coords[0], Segs[i].p1.Coords[1], Segs[i].p1.Coords[2]);
	glVertex3f(Segs[i].p2.Coords[0], Segs[i].p2.Coords[1], Segs[i].p2.Coords[2]);
	int j = (i + 1) % Max;
	glNormal3f(Segs[j].n.Coords[0], Segs[j].n.Coords[1], Segs[j].n.Coords[2]);
	glVertex3f(Segs[j].p1.Coords[0], Segs[j].p1.Coords[1], Segs[j].p1.Coords[2]);
	glVertex3f(Segs[j].p2.Coords[0], Segs[j].p2.Coords[1], Segs[j].p2.Coords[2]);	
    }
    glEnd();
    
    glPopMatrix();
    glutSwapBuffers();
    // calc fps
    Frames++; GLint t = glutGet(GLUT_ELAPSED_TIME); if (t - T0 >= 2500) { float seconds = (t - T0) / 1000.0; float fps = Frames / seconds; printf("%d frames in %6.3f seconds = %6.3f FPS\n", Frames, seconds, fps); T0 = t; Frames = 0; }
}

static void idle(void) { angle += DeAngle; glutPostRedisplay(); }

static void key(unsigned char k, int x, int y)
{
  switch (k)
  {
  case 'z': view_rotz += 5.0; break;
  case 'Z': view_rotz -= 5.0; break;
  case 27: exit(0); break;
  default: return;
  }
  glutPostRedisplay();
}

static void special(int k, int x, int y)
{
  switch (k) {
  case GLUT_KEY_UP: view_rotx += 5.0; break;
  case GLUT_KEY_DOWN: view_rotx -= 5.0; break;
  case GLUT_KEY_LEFT: view_roty += 5.0; break;
  case GLUT_KEY_RIGHT: view_roty -= 5.0; break;
  default: return;
  }
  glutPostRedisplay();
}

/* new window size or exposure */
static void reshape(int width, int height)
{
  GLfloat h = (GLfloat) height / (GLfloat) width;

  glViewport(0, 0, (GLint) width, (GLint) height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -h, h, 5.0, 60.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -40.0);
}

void init()
{
    GLfloat pos1[4] = {4.0, 5.0, 7.0, 0.0};
    GLfloat pos2[4] = {-4.0, 5.0, -3.0, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, pos1);
    glLightfv(GL_LIGHT1, GL_POSITION, pos2);
//    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
    printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
    printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
    printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
  
  
    double fi = 2 * Pi / dMax;
    for (int i = 0; i < Max; i++)
    {
	double fi1 = double(i) * fi;
	TVector v(-sin(fi1), cos(fi1), 0);
	v.Normalize();
	TVector p(R * cos(fi1), R * sin(fi1), 0);
	TVector s(0, 0, W);
	s.Rotate(v, fi1);
	TVector n = (s & v).Normalize();
	Segs[i].p1 = p + s;
	Segs[i].p2 = p - s;
	Segs[i].n = n;
    }  
/*
    double fi = Pi / dMax;
    for (int i = 0; i < Max/2; i++)
    {
	double fi1 = double(i) * fi-Pi/4;
	TVector v(-2.0 * R * sin(2 * fi1) * sin(fi1) + R * cos(2 * fi1) * cos(fi1),
		  -2.0 * R * sin(2 * fi1) * cos(fi1) - R * cos(2 * fi1) * sin(fi1), 0);
	v.Normalize();
	TVector p(R * cos(2 * fi1) * sin(fi1), R * cos(2 * fi1) * cos(fi1), 0);
	TVector s(0, 0, W);
	s.Rotate(v, fi1);
	TVector n = (s & v).Normalize();
	Segs[i].p1 = p + s;
	Segs[i].p2 = p - s;
	Segs[i].n = n;
	
	p = p * (-1.0);
	n = n * (-1.0);

	Segs[Max-1-i].p1 = p+s;
	Segs[Max-1-i].p2 = p-s;
	Segs[Max-1-i].n = n;

    }  
*/

}

void visible(int vis) { if (vis == GLUT_VISIBLE) glutIdleFunc(idle); else glutIdleFunc(NULL); }

// MAIN ======================================
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

  glutInitWindowPosition(0, 0);
  glutInitWindowSize(300, 300);
  glutCreateWindow("Moebius surface");
  init();

  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(key);
  glutSpecialFunc(special);
  glutVisibilityFunc(visible);

  glutMainLoop();
  return 0;    
}
