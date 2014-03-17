#include "MyViewer.h"

using qglviewer::Vec;

MyViewer::MyViewer(QWidget *parent) :
  QGLViewer(parent), animation_type(0)
{
  eye = Vec(0, -1.4, 0.7);
  canvas.p = Vec(0, -0.8, 0);
  canvas.n = Vec(0, 1, 0);
  table.p = Vec(0, 0, -0.8);
  table.n = Vec(0, 0, 1);
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

  camera()->setPosition(Vec(0, -3.3, 0));
  camera()->setUpVector(Vec(0, 0, 1));
  camera()->setViewDirection(Vec(0, 1, 0));
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

  a *= plane_size; b *= plane_size;

  glBegin(GL_POLYGON);
  glVertex3fv(plane.p - a - b);
  glVertex3fv(plane.p + a - b);
  glVertex3fv(plane.p + a + b);
  glVertex3fv(plane.p - a + b);
  glEnd();
}

void MyViewer::drawLine(const MyViewer::Line &line) const
{
  Segment s;
  s.a = line.p - line.d * 100.0;
  s.b = line.p + line.d * 100.0;
  drawSegment(s);
}

void MyViewer::drawSegment(const MyViewer::Segment &segment) const
{
  glBegin(GL_LINES);
  glVertex3fv(segment.a);
  glVertex3fv(segment.b);
  glEnd();
}

void MyViewer::drawCube(const Vec *a) const
{
  glBegin(GL_POLYGON);
  glVertex3fv(a[0]); glVertex3fv(a[1]); glVertex3fv(a[2]); glVertex3fv(a[3]); 
  glEnd();
  glBegin(GL_POLYGON);
  glVertex3fv(a[4]); glVertex3fv(a[5]); glVertex3fv(a[6]); glVertex3fv(a[7]); 
  glEnd();
  glBegin(GL_POLYGON);
  glVertex3fv(a[0]); glVertex3fv(a[1]); glVertex3fv(a[5]); glVertex3fv(a[4]); 
  glEnd();
  glBegin(GL_POLYGON);
  glVertex3fv(a[3]); glVertex3fv(a[2]); glVertex3fv(a[6]); glVertex3fv(a[7]); 
  glEnd();
  glBegin(GL_POLYGON);
  glVertex3fv(a[0]); glVertex3fv(a[4]); glVertex3fv(a[7]); glVertex3fv(a[3]); 
  glEnd();
  glBegin(GL_POLYGON);
  glVertex3fv(a[1]); glVertex3fv(a[5]); glVertex3fv(a[6]); glVertex3fv(a[2]); 
  glEnd();
}

void MyViewer::drawCubeOutline(const Vec *a) const
{
  glBegin(GL_LINE_LOOP);
  glVertex3fv(a[0]); glVertex3fv(a[1]); glVertex3fv(a[2]); glVertex3fv(a[3]); 
  glEnd();
  glBegin(GL_LINE_LOOP);
  glVertex3fv(a[4]); glVertex3fv(a[5]); glVertex3fv(a[6]); glVertex3fv(a[7]); 
  glEnd();
  glBegin(GL_LINES);
  glVertex3fv(a[0]); glVertex3fv(a[4]); glVertex3fv(a[1]); glVertex3fv(a[5]); 
  glVertex3fv(a[2]); glVertex3fv(a[6]); glVertex3fv(a[3]); glVertex3fv(a[7]); 
  glEnd();
}

void MyViewer::draw()
{
  const Vec x(1, 0, 0), y(0, 1, 0), z(0, 0, 1);

  // Draw the planes
  glColor4d(0.6, 0.4, 0.3, 1.0);
  drawPlane(table);
  glColor4d(0.0, 1.0, 0.0, 0.5);
  drawPlane(canvas);

  // Draw the eye
  glDisable(GL_DEPTH_TEST);
  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POINTS);
  glVertex3fv(eye);
  glEnd();
  glEnable(GL_DEPTH_TEST);

  if (animation_type == 0) {
    // Show only the planes and the eye position at startup
    return;
  } else if (animation_type == 1) {
    // The first animation involves a cube and thus needs special handling
    const double cube_size = 0.25;
    Vec a[8], b[8];
    const Vec &point = points.front();
    Line l[8];
    Segment s[8];
    a[0] = point - x * cube_size - y * cube_size - z * cube_size;
    a[1] = point + x * cube_size - y * cube_size - z * cube_size;
    a[2] = point + x * cube_size + y * cube_size - z * cube_size;
    a[3] = point - x * cube_size + y * cube_size - z * cube_size;
    a[4] = point - x * cube_size - y * cube_size + z * cube_size;
    a[5] = point + x * cube_size - y * cube_size + z * cube_size;
    a[6] = point + x * cube_size + y * cube_size + z * cube_size;
    a[7] = point - x * cube_size + y * cube_size + z * cube_size;
    for (size_t i = 0; i < 8; ++i) {
      l[i].p = eye;
      l[i].d = (a[i] - eye).unit();
      s[i].a = eye;
      s[i].b = a[i];
      b[i] = intersectLineWithPlane(l[i], canvas);
    }
    glColor3d(1.0, 1.0, 0.0);
    for (size_t i = 0; i < 8; ++i)
      drawSegment(s[i]);
    glColor3d(1.0, 1.0, 1.0);
    drawCube(a);
    glColor3d(0.0, 0.0, 0.0);
    drawCubeOutline(a);
    drawCubeOutline(b);
    return;
  }

  // Draw all segments
  for (size_t i = 0; i < segments.size(); ++i) {
    Segment segment = segments[i];
    glDisable(GL_DEPTH_TEST);
    glColor3d(0.0, 0.0, 0.0);
    drawSegment(segment);
    
    Segment s;
    Line l(eye, segment.a - eye);
    s.a = intersectLineWithPlane(l, canvas);
    l.d = segment.b - eye;
    s.b = intersectLineWithPlane(l, canvas);
    drawSegment(s);
    glEnable(GL_DEPTH_TEST);
  }

  // Draw all rays
  for (size_t i = 0; i < points.size(); ++i) {
    Segment s;
    s.a = eye;
    s.b = points[i];
    glColor3d(1.0, 1.0, 0.0);
    drawSegment(s);
  }

  if (show_infinite == INF_HORIZON || show_infinite == INF_BOTH) {
    Segment s(Vec(canvas.p[0] - plane_size, canvas.p[1], eye[2]),
              Vec(canvas.p[0] + plane_size, canvas.p[1], eye[2]));
    glColor3d(1.0, 0.0, 0.0);
    drawSegment(s);
  }
  if (show_infinite == INF_FOOT || show_infinite == INF_BOTH) {
    Segment s(Vec(table.p[0] - plane_size, eye[1], table.p[2]),
              Vec(table.p[0] + plane_size, eye[1], table.p[2]));
    glColor3d(1.0, 0.0, 0.0);
    drawSegment(s);
  }
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
    timer->stop();
    timer->disconnect();
    return;
  }

  const double x = (double)animation_counter / 100.0 * 2.0 * M_PI;
  points.front() = Vec(cos(x) * 0.5, 0.2 + sin(x) / 5, sin(x) * 0.5);

  updateGL();
}

void MyViewer::animation2()
{
  if (++animation_counter == 500) {
    timer->stop();
    timer->disconnect();
    return;
  }

  switch(animation_counter) {
  case 1:
    segments.front().a = Vec(0.1, 0.3, -0.8);
    break;
  case 100:
    show_infinite = INF_HORIZON;
    points.clear();
    break;
  case 133:
    points.resize(1);
    segments.clear();
    break;
  case 266:
    show_infinite = INF_NONE;
    segments.resize(1);
    segments.front().a = Vec(0.1, 0.0, -0.8);
    break;
  case 366:
    show_infinite = INF_FOOT;
    points.clear();
    break;
  case 399:
    points.resize(1);
    segments.clear();
    break;
  case 499:
    show_infinite = INF_BOTH;
    points.clear();
    break;
  }

  if (animation_counter < 100) {
    const double x = (double)animation_counter / 100.0;
    // Move point along y
    points.front() = Vec(0.1, 0.3 + x * x * 100.0, -0.8);
    segments.front().b = points.front();
  } else if (133 <= animation_counter && animation_counter < 233) {
    const double x = (double)(animation_counter - 133) / 100.0;
    // Rays on the horizon
    Vec v(canvas.p[0] - plane_size + 2.0 * x * plane_size, canvas.p[1], eye[2]);
    points.front() = eye + (v - eye) * 10.0;
  } else if (266 <= animation_counter && animation_counter < 366) {
    const double x = (double)(animation_counter - 266) / 100.0;
    // Move point along -y
    points.front() = Vec(0.1, x * (eye[1] - epsilon), -0.8);
    segments.front().b = points.front();
  } else if (399 <= animation_counter && animation_counter < 499) {
    const double x = (double)(animation_counter - 399) / 100.0;
    // Rays on the footline
    Vec v(table.p[0] - plane_size + 2.0 * x * plane_size, eye[1], table.p[2]);
    points.front() = eye + (v - eye) * 10.0;
  } else if (animation_counter == 499) {
    // Show all kinds of points
    points.push_back(Vec(0.1, 0.9, -0.8));
    Vec v;
    v = Vec(0.1, -0.2, -0.8);
    v = intersectLineWithPlane(Line(eye, v - eye), canvas);
    points.push_back(v);
    v = Vec(0.1, -0.9, -0.8);
    points.push_back(v);
    v = intersectLineWithPlane(Line(eye, v - eye), canvas);
    points.push_back(v);
  } else {
    // Just wait
  }

  updateGL();
}

void MyViewer::animation3()
{
  if (++animation_counter == 100) {
    timer->stop();
    timer->disconnect();
    return;
  }

  const double x = (double)animation_counter / 100.0;
  const Segment &segment = segments.front();
  points.front() = segment.a + x * (segment.b - segment.a);
  updateGL();
}

void MyViewer::animation4()
{
  if (++animation_counter == 100) {
    timer->stop();
    timer->disconnect();
    return;
  }

  updateGL();
}

void MyViewer::animation5()
{
  if (++animation_counter == 100) {
    timer->stop();
    timer->disconnect();
    return;
  }

  updateGL();
}

void MyViewer::animation6()
{
  if (++animation_counter == 100) {
    timer->stop();
    timer->disconnect();
    return;
  }

  updateGL();
}

void MyViewer::animate(int type)
{
  if (timer->isActive())           // Already running an animation
    return;

  animation_type = type;

  switch(type) {
  case 1:
    eye = Vec(-0.3, -2.0, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    points.resize(1);
    show_infinite = INF_NONE;

    camera()->setPosition(Vec(3.28356, 0.708553, 0.581566));
    camera()->setUpVector(Vec(-0.188367, -0.0818547, 0.978682));
    camera()->setViewDirection(Vec(-0.947526, -0.246932, -0.203023));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation1()));
    timer->start(50);
    break;
  case 2:
    eye = Vec(0, -0.4, -0.2);   
    canvas.p = Vec(0, 0, -0.5);
    segments.resize(1);
    points.resize(1);
    show_infinite = INF_NONE;

    camera()->setPosition(Vec(3.14783, -1.18086, -0.14118));
    camera()->setUpVector(Vec(-0.118301, 0.0450942, 0.991953));
    camera()->setViewDirection(Vec(-0.904035, 0.40835, -0.126379));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation2()));
    timer->start(50);
    break;
  case 3:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    segments.push_back(Segment(Vec(-0.3, 0.5, -0.8), Vec(0.7, -0.6, -0.8)));
    points.resize(1);
    show_infinite = INF_NONE;

    camera()->setPosition(Vec(3.38021, -1.95618, 0.171107));
    camera()->setUpVector(Vec(-0.0171574, 0.0576644, 0.998189));
    camera()->setViewDirection(Vec(-0.864684, 0.500405, -0.0437706));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation3()));
    timer->start(50);
    break;
  case 4:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    points.clear();
    show_infinite = INF_NONE;

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation4()));
    timer->start(50);
    break;
  case 5:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    points.clear();
    show_infinite = INF_NONE;

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation4()));
    timer->start(50);
    break;
  case 6:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    points.clear();
    show_infinite = INF_NONE;

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation4()));
    timer->start(50);
    break;
  case 7:
    std::cout << "camera()->setPosition(Vec(" << camera()->position()[0] << ", "
              << camera()->position()[1] << ", " << camera()->position()[2]
              << "));" << std::endl;
    std::cout << "camera()->setUpVector(Vec(" << camera()->upVector()[0] << ", "
              << camera()->upVector()[1] << ", " << camera()->upVector()[2]
              << "));" << std::endl;
    std::cout << "camera()->setViewDirection(Vec(" << camera()->viewDirection()[0] << ", "
              << camera()->viewDirection()[1] << ", " << camera()->viewDirection()[2]
              << "));" << std::endl;
    break;
  default:
    std::cerr << "Animation " << type << " is not implemented yet." << std::endl;
  }
}
