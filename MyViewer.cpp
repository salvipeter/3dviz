#include "MyViewer.h"

#ifdef WIN32
 #include <windows.h>
#else
 #include <unistd.h>
#endif

void sleepMilliSeconds(size_t ms)
{
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
}

using qglviewer::Vec;

MyViewer::MyViewer(QWidget *parent) :
  QGLViewer(parent)
{
  eye = Vec(-0.5, -0.5, -0.5);
}

MyViewer::~MyViewer()
{
}

void MyViewer::init()
{
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  glClearColor(0.5, 0.5, 1.0, 1.0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  glDisable(GL_LIGHTING);
}

void MyViewer::draw()
{
  glColor4d(1.0, 0.0, 0.0, 0.5);
  glBegin(GL_POLYGON);
  glVertex2d(-1.0, -1.0);
  glVertex2d(1.0, -1.0);
  glVertex2d(1.0, 1.0);
  glVertex2d(-1.0, 1.0);
  glEnd();

  glColor4d(0.0, 1.0, 0.0, 0.5);
  glBegin(GL_POLYGON);
  glVertex3d(0.0, -1.0, -1.0);
  glVertex3d(0.0, 1.0, -1.0);
  glVertex3d(0.0, 1.0, 1.0);
  glVertex3d(0.0, -1.0, 1.0);
  glEnd();

  glDisable(GL_DEPTH_TEST);

  glLineWidth(2.0);
  glColor3d(0.0, 0.0, 0.0);
  glBegin(GL_LINES);
  glVertex2d(0.3, 0.5);
  glVertex2d(0.7, 0.6);
  glEnd();

  glPointSize(10.0);
  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POINTS);
  glVertex3fv(eye);
  glEnd();
  glPointSize(1.0);

  glEnable(GL_DEPTH_TEST);
}

void MyViewer::animate(int type)
{
  switch(type) {
  case 1:
    for (double x = 0; x < 0.5; x += 0.005) {
      eye += Vec(x, x, 0.0);
      updateGL();
      sleepMilliSeconds(50);
    }
    break;
  default:
    std::cerr << "Animation " << type << " is not implemented yet." << std::endl;
  }
}
