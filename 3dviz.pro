# -*- mode: Makefile -*-

TARGET = 3dviz
CONFIG *= qt opengl release
QT *= gui widgets opengl openglwidgets xml

HEADERS = MyWindow.h MyViewer.h
SOURCES = MyWindow.cpp MyViewer.cpp main.cpp

win32: INCLUDEPATH += "$$(QGLVIEWER_DIR)/"
mac: INCLUDEPATH += "/Users/salvi/share/libQGLViewer"

unix:!mac: LIBS *= -lQGLViewer-qt5 -lGLU
mac: LIBS *= -F"/Users/salvi/share/libQGLViewer/QGLViewer/" -framework QGLViewer
# QGLVIEWER_DIR is an environment variable
win32: LIBS *= -L"$$(QGLVIEWER_DIR)/QGLViewer/release/" -lQGLViewer2
