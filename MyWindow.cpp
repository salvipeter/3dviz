#include <QHBoxLayout>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

#include "MyWindow.h"

MyWindow::MyWindow() : QWidget()
{
  setWindowTitle(tr("3D Vizualiz�ci�"));

  viewer = new MyViewer(this);
  QVBoxLayout *vbox = new QVBoxLayout;
  QHBoxLayout *hbox1 = new QHBoxLayout, *hbox2 = new QHBoxLayout;

  QString texts[] = { "", tr("Kocka"), tr("Pontok el�l"), tr("Pontok h�tul"), tr("Pontok k�pe"),
                      tr("Egyenes"), tr("Sug�rs�k"), tr("P�rhuzamosok I"),
                      tr("P�rhuzamosok II"), tr("P�rhuzamosok s�kjai"), tr("Kamera info") };
  QSignalMapper *mapper = new QSignalMapper(this);
  for (size_t i = 1; i < 10; ++i) {
    QPushButton *button = new QPushButton(texts[i]);
    connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(button, i);
    if (i <= 5)
      hbox1->addWidget(button);
    else
      hbox2->addWidget(button);
  }
  connect(mapper, SIGNAL(mapped(int)), viewer, SLOT(animate(int)));

  vbox->addWidget(viewer);
  vbox->addLayout(hbox1);
  vbox->addLayout(hbox2);
  setLayout(vbox);
  showMaximized();
}

MyWindow::~MyWindow()
{
}
