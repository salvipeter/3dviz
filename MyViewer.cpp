#include "MyViewer.h"

using qglviewer::Vec;

MyViewer::MyViewer(QWidget *parent) :
  QGLViewer(parent)
{
  eye = Vec(0, -1.4, 0.7);
  canvas.p = Vec(0, -0.8, 0);
  canvas.n = Vec(0, 1, 0);
  table.p = Vec(0, 0, -0.8);
  table.n = Vec(0, 0, 1);
  segment.a = Vec(-0.3, 0.5, -0.8);
  segment.b = Vec(0.7, -0.6, -0.8);
  point = segment.a;
  timer = new QTimer(this);
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

  glLineWidth(2.0);
  glPointSize(10.0);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  glDisable(GL_LIGHTING);

  camera()->setPosition(Vec(3.38021, -1.95618, 0.171107));
  camera()->setUpVector(Vec(-0.0171574, 0.0576644, 0.998189));
  camera()->setViewDirection(Vec(-0.864684, 0.500405, -0.0437706));
}

void MyViewer::drawPlane(const MyViewer::Plane &plane) const
{
  Vec a, b;
  if (plane.n[0] == 0) {
    a = Vec(1, 0, 0);
    if (plane.n[1] == 0)
      b = Vec(0, 1, 0);
    else
      b = Vec(0, 0, 1);
  } else {
    a = Vec(0, 1, 0);
    b = Vec(0, 0, 1);
  }

  const double size = 1.0;
  a *= size; b *= size;

  glBegin(GL_POLYGON);
  glVertex3fv(plane.p - a - b);
  glVertex3fv(plane.p + a - b);
  glVertex3fv(plane.p + a + b);
  glVertex3fv(plane.p - a + b);
  glEnd();
}

void MyViewer::drawSegment(const MyViewer::Segment &segment) const
{
  glBegin(GL_LINES);
  glVertex3fv(segment.a);
  glVertex3fv(segment.b);
  glEnd();
}

void MyViewer::draw()
{
  glColor4d(0.0, 1.0, 0.0, 0.5);
  drawPlane(canvas);

  glColor4d(1.0, 1.0, 0.0, 0.5);
  drawPlane(table);

  glDisable(GL_DEPTH_TEST);

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POINTS);
  glVertex3fv(eye);
  glEnd();

  glColor3d(0.0, 0.0, 0.0);
  drawSegment(segment);

  Segment s;
  Line l;
  l.p = eye;
  l.d = segment.a - eye;
  s.a = intersectLineWithPlane(l, canvas);
  l.d = segment.b - eye;
  s.b = intersectLineWithPlane(l, canvas);
  drawSegment(s);

  glEnable(GL_DEPTH_TEST);

  s.a = eye;
  s.b = point;
  glColor3d(1.0, 1.0, 0.0);
  drawSegment(s);
}

Vec MyViewer::intersectLineWithPlane(const MyViewer::Line &line, const MyViewer::Plane &plane)
{
  const double dn = line.d * plane.n;
  if (fabs(dn) < epsilon)
    return Vec(0, 0, 0);

  const double x = ((plane.p - line.p) * plane.n) / dn;

  return line.p + line.d * x;
}

void MyViewer::animation1()
{
  if (++animation_counter == 100) {
    timer->disconnect();
    return;
  }

  const double x = (double)animation_counter / 100.0;
  point = segment.a + x * (segment.b - segment.a);
  updateGL();
}

void MyViewer::animate(int type)
{
  switch(type) {
  case 1:
    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation1()));
    timer->start(50);
    break;
  case 4:
    std::cout << "Position: " << camera()->position()[0] << ", " << camera()->position()[1]
              << ", " << camera()->position()[2] << std::endl;
    std::cout << "upVector: " << camera()->upVector()[0] << ", " << camera()->upVector()[1]
              << ", " << camera()->upVector()[2] << std::endl;
    std::cout << "viewDir: " << camera()->viewDirection()[0] << ", "
              << camera()->viewDirection()[1] << ", " << camera()->viewDirection()[2] << std::endl;
    break;
  default:
    std::cerr << "Animation " << type << " is not implemented yet." << std::endl;
  }
}
