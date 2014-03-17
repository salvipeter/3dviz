// -*- mode: c++ -*-
#pragma once

#include <QMainWindow>

#include "MyViewer.h"

class MyWindow : public QWidget
{
  Q_OBJECT

public:
  MyWindow();
  ~MyWindow();

private:
  MyViewer *viewer;
};
