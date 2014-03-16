# -*- mode: Makefile -*-

TARGET = 3dviz
CONFIG *= qt opengl release
QT *= opengl xml

HEADERS = MyWindow.h MyViewer.h
SOURCES = MyWindow.cpp MyViewer.cpp main.cpp

LIBS *= -lqglviewer-qt4
