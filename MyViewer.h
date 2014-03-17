// -*- mode: c++ -*-
#pragma once

#include <QGLViewer/qglviewer.h>

using qglviewer::Vec;

class QTimer;

class MyViewer : public QGLViewer
{
  Q_OBJECT

public:
  MyViewer(QWidget *parent);
  virtual ~MyViewer();

  struct Plane {
    Vec p, n;
    Plane() { }
    Plane(Vec p, Vec n) : p(p), n(n) { }
  };
  struct Line {
    Vec p, d;
    Line() { }
    Line(Vec p, Vec d) : p(p), d(d) { }
  };
  struct Segment {
    Vec a, b;
    Segment() { }
    Segment(Vec a, Vec b) : a(a), b(b) { }
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
  void drawLine(const Line &line) const;    // only handles lines in xyz planes
  void drawSegment(const Segment &segment) const;
  static Vec intersectLineWithPlane(const Line &line, const Plane &plane);

  static const double epsilon = 1.0e-7;
  Vec eye;
  Plane canvas, table;
  Line line;
  Segment segment;
  Vec point;
  QTimer *timer;
  size_t animation_type, animation_counter;
};
