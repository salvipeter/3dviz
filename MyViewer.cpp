#include "MyViewer.h"

#include "GL/glu.h"

using qglviewer::Vec;

const double MyViewer::epsilon = 1.0e-7;
const double MyViewer::plane_size = 1.15;

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
  glEnable(GL_LINE_SMOOTH);

  camera()->setPosition(Vec(0, -3.3, 0));
  camera()->setUpVector(Vec(0, 0, 1));
  camera()->setViewDirection(Vec(0, 1, 0));

  camera()->setSceneBoundingBox(Vec(-2.0, -100.0, -4.0),
                                Vec(2.0, 100.0, 4.0));
}

void MyViewer::drawPlane(const MyViewer::Plane &plane) const
{
  Vec a, b;
  if (plane.n[0] == 0) {
    a = Vec(1, 0, 0);
    if (plane.n[1] == 0)
      b = Vec(0, 100, 0);
    else
      b = Vec(0, 0, 100);
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

void MyViewer::drawGeneralPlane(const MyViewer::Plane &plane) const
{
  // Assume that the normal is not in z
  const double bottom = table.p[2] - 0.1, top = eye[2] + 0.1;
  const double left = eye[1] - 0.1, right = table.p[1] + plane_size + 0.1;

  glBegin(GL_POLYGON);
  glVertex3fv(intersectLineWithPlane(Line(Vec(0, left, top), Vec(1, 0, 0)), plane));
  glVertex3fv(intersectLineWithPlane(Line(Vec(0, right, top), Vec(1, 0, 0)), plane));
  glVertex3fv(intersectLineWithPlane(Line(Vec(0, right, bottom), Vec(1, 0, 0)), plane));
  glVertex3fv(intersectLineWithPlane(Line(Vec(0, left, bottom), Vec(1, 0, 0)), plane));
  glEnd();

  glColor3d(0.8, 0.2, 0.2);
  glBegin(GL_LINES);
  glVertex3fv(intersectLineWithPlane(Line(Vec(0, left, eye[2]), Vec(1, 0, 0)), plane));
  glVertex3fv(intersectLineWithPlane(Line(Vec(0, right, eye[2]), Vec(1, 0, 0)), plane));
  glEnd();
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

void MyViewer::drawSphere(const Vec &p, double r) const
{
  GLUquadricObj *quadric = gluNewQuadric();
  gluQuadricNormals(quadric, GLU_SMOOTH);

  glPushMatrix();
  glTranslatef(p[0], p[1], p[2]);
  glEnable(GL_LIGHTING);
  gluSphere(quadric, r, 20, 20);
  glDisable(GL_LIGHTING);
  glPopMatrix();

  gluDeleteQuadric(quadric);
}

void MyViewer::draw()
{
  const Vec x(1, 0, 0), y(0, 1, 0), z(0, 0, 1);

  // Draw the eye
  glColor3d(0.0, 1.0, 1.0);
  drawSphere(eye, 0.025);

  if (animation_type == 1) {
    // The first animation involves a cube and thus needs special handling
    const double cube_size = 0.25;
    Vec a[8], b[8];
    const Vec &point = points[0];
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
  }

  if (animation_type < 2) {
    // Draw the main planes
    glColor4d(0.6, 0.4, 0.3, 0.7);
    drawPlane(table);
    glColor4d(0.0, 1.0, 0.0, 0.5);
    drawPlane(canvas);
    return;
  }

  // Draw all segments
  for (size_t i = 0; i < segments.size(); ++i) {
    Segment segment = segments[i];
    // glDisable(GL_DEPTH_TEST);
    glColor3d(0.0, 0.0, 0.0);
    drawSegment(segment);
    
    Segment s;
    Line l(eye, segment.a - eye);
    s.a = intersectLineWithPlane(l, canvas);
    l.d = segment.b - eye;
    s.b = intersectLineWithPlane(l, canvas);
    drawSegment(s);
    // glEnable(GL_DEPTH_TEST);
  }

  // Draw all rays
  for (size_t i = 0; i < points.size(); ++i) {
    Segment s;
    s.a = eye;
    s.b = points[i];
    glColor3d(1.0, 1.0, 0.0);
    drawSegment(s);
  }

  // Draw flash points
  for (size_t i = 0; i < flashes.size(); ++i) {
    glColor3d(0.8, 0.8, 0.8);
    glBegin(GL_POINTS);
    glVertex3fv(flashes[i]);
    glEnd();
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

  // Draw all secondary planes
  for (size_t i = 0; i < planes.size(); ++i) {
    glColor4d(1.0, 1.0, 0.0, 0.5);
    drawGeneralPlane(planes[i]);
  }

  // Draw the main planes
  glColor4d(0.6, 0.4, 0.3, 0.7);
  drawPlane(table);
  glColor4d(0.0, 1.0, 0.0, 0.5);
  drawPlane(canvas);
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

  const double x = (double)animation_counter / 99.0 * 2.0 * M_PI;
  points[0] = Vec(cos(x) * 0.5, 0.2 + sin(x) / 5, sin(x) * 0.5);

  updateGL();
}

void MyViewer::animation2()
{
  if (++animation_counter == 300) {
    points.clear();
    updateGL();
    timer->stop();
    timer->disconnect();
    return;
  }

  switch(animation_counter) {
  case 1:
    segments[0].a = Vec(0.1, 0.3, -0.8);
    break;
  case 166:
    show_infinite = INF_HORIZON;
    points.clear();
    break;
  case 200:
    points.resize(1);
    segments.clear();
    break;
  }

  if (animation_counter < 166) {
    const double x = (double)animation_counter / 165.0;
    // Move point along y
    points[0] = Vec(0.1, 0.3 + x * x * 100.0, -0.8);
    segments[0].b = points[0];
  } else if (200 <= animation_counter && animation_counter < 300) {
    const double x = (double)(animation_counter - 200) / 99.0;
    // Rays on the horizon
    Vec v(canvas.p[0] - plane_size + 2.0 * x * plane_size, canvas.p[1], eye[2]);
    points[0] = eye + (v - eye) * 100.0;
  }

  updateGL();
}

void MyViewer::animation3()
{
  if (++animation_counter == 300) {
    points.clear();
    updateGL();
    timer->stop();
    timer->disconnect();
    return;
  }

  switch(animation_counter) {
  case 1:
    segments[0].a = Vec(0.1, 0.0, -0.8);
    break;
  case 166:
    show_infinite = INF_FOOT;
    points.clear();
    break;
  case 200:
    points.resize(1);
    segments.clear();
    break;
  }

  if (animation_counter < 166) {
    const double x = (double)animation_counter / 165.0;
    // Move point along -y
    points[0] = Vec(0.1, x * (eye[1] + 100 * epsilon), -0.8);
    points[0] = intersectLineWithPlane(Line(eye, points[0] - eye), canvas);
    segments[0].b = points[0];
  } else if (200 <= animation_counter && animation_counter < 300) {
    const double x = (double)(animation_counter - 200) / 99.0;
    // Rays on the footline
    Vec v(table.p[0] - plane_size + 2.0 * x * plane_size, eye[1], table.p[2]);
    points[0] = eye + (v - eye) * 100.0;
  }

  updateGL();
}

void MyViewer::animation4()
{
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

  updateGL();

  timer->stop();
  timer->disconnect();
}

void MyViewer::animation5()
{
  if (++animation_counter == 300) {
    points.clear();
    updateGL();
    timer->stop();
    timer->disconnect();
    return;
  }

  if (animation_counter == 1)
    segments[0].a = Vec(0.1, -10.0, -0.8);
  else if (animation_counter == 150) {
    points.resize(2);
    segments.push_back(Segment(Vec(0.1, eye[1] + 100.0 * epsilon, -0.8), Vec(0.1, 10.0, -0.8)));
  }

  if (animation_counter < 100) {
    const double x = (double)animation_counter / 99.0;
    Segment &segment = segments[0];
    points[0] = Vec(0.1, -10.0 + x * (eye[1] - 100.0 * epsilon + 10), -0.8);
    points[1] = intersectLineWithPlane(Line(eye, points[0] - eye), canvas);
    segment.b = points[0];
  } else if (150 <= animation_counter) {
    const double x = (double)(animation_counter - 150) / 149.0;
    Segment &segment = segments[1];
    points[0] = Vec(0.1, eye[1] + 100.0 * epsilon + x * 10.0, -0.8);
    points[1] = intersectLineWithPlane(Line(eye, points[0] - eye), canvas);
    segment.b = points[0];
  } else {
    // Wait
  }

  updateGL();
}

void MyViewer::animation6()
{
  if (++animation_counter == 351) {
    points.clear();
    updateGL();
    timer->stop();
    timer->disconnect();
    return;
  }

  switch(animation_counter) {
  case 150:
    points.clear();
    planes.push_back(Plane(segments[0], eye));
    break;
  case 201:
    points.resize(1);
    flashes.clear();
    break;
  }

  if (animation_counter < 150) {
    const double x = (double)animation_counter / 149.0;
    const Segment &segment = segments[0];
    points[0] = segment.a + x * x * (segment.b - segment.a);
    if (animation_counter % 10 == 0) {
      flashes.push_back(points[0]);
      flashes.push_back(intersectLineWithPlane(Line(eye, points[0] - eye), canvas));
    }
  } else if (201 <= animation_counter) {
    const double x = (double)(animation_counter - 201) / 149.0;
    const Segment &segment = segments[0];
    points[0] = segment.a + x * x * (segment.b - segment.a);
    if (animation_counter % 10 == 0) {
      flashes.push_back(points[0]);
      flashes.push_back(intersectLineWithPlane(Line(eye, points[0] - eye), canvas));
    }
  }

  updateGL();
}

void MyViewer::animation7()
{
  if (++animation_counter == 200) {
    points.clear();
    updateGL();
    timer->stop();
    timer->disconnect();
    return;
  }

  const double x = (double)animation_counter / 199.0;
  for (size_t i = 0; i < segments.size(); ++i) {
    points[i] = segments[i].a + x * 2.0 * plane_size * Vec(1.0, 0.0, 0.0);
    segments[i].b = points[i];
  }

  updateGL();
}

void MyViewer::animation8()
{
  if (++animation_counter == 200) {
    points.clear();
    updateGL();
    timer->stop();
    timer->disconnect();
    return;
  }

  const double x = (double)animation_counter / 199.0;
  for (size_t i = 0; i < segments.size(); ++i) {
    points[i] = segments[i].a + x * x * 100.0 * Vec(0.1, 1.0, 0.0);
    segments[i].b = points[i];
  }

  updateGL();
}

void MyViewer::animation9()
{
  if (++animation_counter == 7) {
    timer->stop();
    timer->disconnect();
    return;
  }

  switch(animation_counter) {
  case 1:
    planes[0] = Plane(segments[2], eye);
    break;
  case 2:
    planes[0] = Plane(segments[1], eye);
    break;
  case 3:
    planes[0] = Plane(segments[0], eye);
    break;
  case 4:
    planes.push_back(Plane(segments[1], eye));
    break;
  case 5:
    planes.push_back(Plane(segments[2], eye));
    break;
  case 6:
    planes.push_back(Plane(segments[3], eye));
    break;
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
    planes.clear();
    flashes.clear();
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
    planes.clear();
    flashes.clear();
    show_infinite = INF_NONE;

    camera()->setPosition(Vec(3.14783, -1.18086, -0.14118));
    camera()->setUpVector(Vec(-0.118301, 0.0450942, 0.991953));
    camera()->setViewDirection(Vec(-0.904035, 0.40835, -0.126379));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation2()));
    timer->start(50);
    break;
  case 3:
    eye = Vec(0, -0.4, -0.2);   
    canvas.p = Vec(0, 0, -0.5);
    segments.resize(1);
    points.resize(1);
    planes.clear();
    flashes.clear();
    show_infinite = INF_NONE;

    camera()->setPosition(Vec(3.14783, -1.18086, -0.14118));
    camera()->setUpVector(Vec(-0.118301, 0.0450942, 0.991953));
    camera()->setViewDirection(Vec(-0.904035, 0.40835, -0.126379));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation3()));
    timer->start(50);
    break;
  case 4:
    eye = Vec(0, -0.4, -0.2);   
    canvas.p = Vec(0, 0, -0.5);
    segments.resize(1);
    points.clear();
    planes.clear();
    flashes.clear();
    show_infinite = INF_BOTH;

    camera()->setPosition(Vec(3.14783, -1.18086, -0.14118));
    camera()->setUpVector(Vec(-0.118301, 0.0450942, 0.991953));
    camera()->setViewDirection(Vec(-0.904035, 0.40835, -0.126379));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation4()));
    timer->start(50);
    break;
  case 5:
    eye = Vec(0, -0.3, -0.2);
    canvas.p = Vec(0, 0, -0.5);
    segments.resize(1);
    points.resize(2);
    planes.clear();
    flashes.clear();
    show_infinite = INF_BOTH;

    camera()->setPosition(Vec(3.00051, -0.839958, -0.225271));
    camera()->setUpVector(Vec(-0.0644347, 0.0180984, 0.997758));
    camera()->setViewDirection(Vec(-0.95405, 0.292081, -0.0669102));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation5()));
    timer->start(50);
    break;
  case 6:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    segments.push_back(Segment(Vec(0.6, -0.6, -0.8), Vec(0.6, 100.0, -0.8)));
    points.resize(1);
    planes.clear();
    flashes.clear();
    show_infinite = INF_HORIZON;

    camera()->setPosition(Vec(4.69493, -4.76521, 2.54081));
    camera()->setUpVector(Vec(-0.305691, 0.213557, 0.927872));
    camera()->setViewDirection(Vec(-0.612017, 0.702455, -0.363307));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation6()));
    timer->start(50);
    break;
  case 7:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    {
      const double x = table.p[0] - plane_size;
      segments.push_back(Segment(Vec(x, -0.5, -0.8), Vec(x, -0.5, -0.8)));
      segments.push_back(Segment(Vec(x, -0.2, -0.8), Vec(x, -0.2, -0.8)));
      segments.push_back(Segment(Vec(x, 0.1, -0.8), Vec(x, 0.1, -0.8)));
      segments.push_back(Segment(Vec(x, 0.4, -0.8), Vec(x, 0.4, -0.8)));
      segments.push_back(Segment(Vec(x, 0.7, -0.8), Vec(x, 0.7, -0.8)));
    }
    points.resize(5);
    planes.clear();
    flashes.clear();
    show_infinite = INF_NONE;

    camera()->setPosition(Vec(3.19036, 0.0388229, 0.913055));
    camera()->setUpVector(Vec(-0.356683, -0.0835291, 0.930484));
    camera()->setViewDirection(Vec(-0.929941, -0.0635316, -0.362178));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation7()));
    timer->start(50);
    break;
  case 8:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    segments.push_back(Segment(Vec(-0.5, -0.5, -0.8), Vec(-0.5, -0.5, -0.8)));
    segments.push_back(Segment(Vec(-0.2, -0.5, -0.8), Vec(-0.2, -0.5, -0.8)));
    segments.push_back(Segment(Vec(0.3, -0.5, -0.8), Vec(0.3, -0.5, -0.8)));
    segments.push_back(Segment(Vec(0.6, -0.5, -0.8), Vec(0.6, -0.5, -0.8)));
    points.resize(4);
    planes.clear();
    flashes.clear();
    show_infinite = INF_HORIZON;

    camera()->setPosition(Vec(3.06833, 0.110576, 0.896283));
    camera()->setUpVector(Vec(-0.321134, -0.0518585, 0.945613));
    camera()->setViewDirection(Vec(-0.943686, -0.0663609, -0.324119));

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation8()));
    timer->start(50);
    break;
  case 9:
    eye = Vec(0, -1.4, 0.7);
    canvas.p = Vec(0, -0.8, 0);
    segments.clear();
    segments.push_back(Segment(Vec(-0.5, -0.5, -0.8), Vec(9.5, 99.5, -0.8)));
    segments.push_back(Segment(Vec(-0.2, -0.5, -0.8), Vec(9.8, 99.5, -0.8)));
    segments.push_back(Segment(Vec(0.3, -0.5, -0.8), Vec(10.3, 99.5, -0.8)));
    segments.push_back(Segment(Vec(0.6, -0.5, -0.8), Vec(10.6, 99.5, -0.8)));
    points.clear();
    planes.clear();
    planes.push_back(Plane(segments[3], eye));
    flashes.clear();
    show_infinite = INF_HORIZON;

    camera()->setPosition(Vec(3.06833, 0.110576, 0.896283));
    camera()->setUpVector(Vec(-0.321134, -0.0518585, 0.945613));
    camera()->setViewDirection(Vec(-0.943686, -0.0663609, -0.324119));
    updateGL();

    animation_counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(animation9()));
    timer->start(1500);
    break;
  case 10:
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
