#include "../headers/page.h"

#include <QGuiApplication>
#include <QScreen>

Page::Page(QObject *parent) : QObject(parent) {
  m_scene = new QGraphicsScene(this);
  m_undoStack = new QUndoStack(this);
  m_scene->setSceneRect(-10000, -10000, 20000, 20000);
  m_zoomFactor = 1.0;
}
