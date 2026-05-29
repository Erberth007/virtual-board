#include "../headers/page.h"

#include <QGuiApplication>
#include <QScreen>

Page::Page(QObject *parent) : QObject(parent) {
  m_scene = new QGraphicsScene(this);
  m_undoStack = new QUndoStack(this);

  QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
  m_scene->setSceneRect(0, 0, screenGeometry.width(), screenGeometry.height());
}
