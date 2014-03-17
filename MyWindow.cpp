#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "MyWindow.h"

MyWindow::MyWindow() : QWidget()
{
  setWindowTitle(tr("3D Vizualizáció"));

  viewer = new MyViewer(this);
  QVBoxLayout *vbox = new QVBoxLayout;
  QHBoxLayout *hbox = new QHBoxLayout;

  QString texts[] = { "", tr("Kocka"), tr("Pontok képe"), tr("Egyenes képe"),
                      tr("Sugársíkok"), tr("Párhuzamosok"), tr("Párhuzamosok síkjai"),
                      tr("Kamera info") };
  QSignalMapper *mapper = new QSignalMapper(this);
  for (size_t i = 1; i <= 7; ++i) {
    QPushButton *button = new QPushButton(texts[i]);
    connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button, i);
    hbox->addWidget(button);
  }
  connect(mapper, SIGNAL(mapped(int)), viewer, SLOT(animate(int)));

  vbox->addWidget(viewer);
  vbox->addLayout(hbox);
  setLayout(vbox);
  showMaximized();
}

MyWindow::~MyWindow()
{
}
