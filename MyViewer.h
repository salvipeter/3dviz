// -*- mode: c++ -*-
#pragma once

#include <vector>

#include <QGLViewer/qglviewer.h>

using qglviewer::Vec;

class QTimer;

class MyViewer : public QGLViewer
{
  Q_OBJECT

public:
  MyViewer(QWidget *parent);
  virtual ~MyViewer();

  struct Segment {
    Vec a, b;
    Segment() { }
    Segment(Vec a, Vec b) : a(a), b(b) { }
  };
  struct Plane {
    Vec p, n;
    Plane() { }
    Plane(Vec p, Vec n) : p(p), n(n) { }
    Plane(Segment s, Vec p) : p((s.a + p)/2.0) {
      // Assume that s.a is not p
      n = ((s.a - p) ^ (s.b - s.a)).unit();
    }
  };
  struct Line {
    Vec p, d;
    Line() { }
    Line(Vec p, Vec d) : p(p), d(d) { }
  };

public slots:
  void animate(int type);
  void animation1();
  void animation2();
  void animation3();
  void animation4();
  void animation5();
  void animation6();

protected:
  virtual void init();
  virtual void draw();

private:
  void drawPlane(const Plane &plane) const; // only handles xyz planes
  void drawGeneralPlane(const Plane &plane) const;
  void drawSegment(const Segment &segment) const;
  void drawCube(const Vec *vertices) const;
  void drawCubeOutline(const Vec *vertices) const;
  static Vec intersectLineWithPlane(const Line &line, const Plane &plane);

  static const double epsilon = 1.0e-7, plane_size = 1.0;
  Vec eye;
  Plane canvas, table;
  std::vector<Segment> segments;
  std::vector<Vec> points;
  std::vector<Plane> planes;
  QTimer *timer;
  size_t animation_type, animation_counter;
  enum InfinitePointType { INF_NONE, INF_HORIZON, INF_FOOT, INF_BOTH } show_infinite;
};
