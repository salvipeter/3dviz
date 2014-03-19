# -*- mode: Makefile -*-

TARGET = 3dviz
CONFIG *= qt opengl release
QT *= opengl xml

HEADERS = MyWindow.h MyViewer.h
SOURCES = MyWindow.cpp MyViewer.cpp main.cpp

win32: INCLUDE += -I"$$(QGLVIEWER_DIR)/";

unix: LIBS *= -lqglviewer-qt4 -lGLU
# QGLVIEWER_DIR is an environment variable
win32: LIBS *= -L"$$(QGLVIEWER_DIR)/QGLViewer/release/" -lQGLViewer2
