// -*- mode: c++ -*-
#pragma once

#include <QMainWindow>

#include "MyViewer.h"

class QApplication;

class MyWindow : public QWidget
{
  Q_OBJECT

public:
  MyWindow(QApplication *parent);
  ~MyWindow();

private:
  QApplication *parent;
  MyViewer *viewer;
};
