#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "MyWindow.h"

MyWindow::MyWindow(QApplication *parent) : QWidget(), parent(parent)
{
  setWindowTitle(tr("3D Visualization"));

  viewer = new MyViewer(this);
  QVBoxLayout *vbox = new QVBoxLayout;
  QHBoxLayout *hbox = new QHBoxLayout;

  QSignalMapper *mapper = new QSignalMapper(this);
  for (size_t i = 1; i <= 4; ++i) {
    QPushButton *button = new QPushButton(QString(tr("Animation %1")).arg(i));
    connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button, i);
    hbox->addWidget(button);
  }
  connect(mapper, SIGNAL(mapped(int)), viewer, SLOT(animate(int)));

  vbox->addWidget(viewer);
  vbox->addLayout(hbox);
  setLayout(vbox);
}

MyWindow::~MyWindow()
{
}
