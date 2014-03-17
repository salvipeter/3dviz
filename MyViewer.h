// -*- mode: c++ -*-
#pragma once

#include <string>

#include <QGLViewer/qglviewer.h>

using qglviewer::Vec;

class MyViewer : public QGLViewer
{
  Q_OBJECT

public:
  MyViewer(QWidget *parent);
  virtual ~MyViewer();

  struct Plane { Vec p, n; };
  struct Line { Vec p, d; };
  struct Segment { Vec a, b; };

public slots:
  void animate(int type);

protected:
  virtual void init();
  virtual void draw();

private:
  void drawPlane(const Plane &plane) const; // only handles xyz planes
  void drawSegment(const Segment &segment) const;
  static Vec intersectLineWithPlane(const Line &line, const Plane &plane);

  static const double epsilon = 1.0e-7;
  Vec eye;
  Plane canvas, table;
  Segment segment;
  Vec point;
};
