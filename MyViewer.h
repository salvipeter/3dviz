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

public slots:
  void animate(int type);

protected:
  virtual void init();
  virtual void draw();

private:
  Vec eye;
};
