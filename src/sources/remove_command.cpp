#include "../headers/remove_command.h"
#include <QDebug>

RemoveCommand::RemoveCommand(QGraphicsItem *item, QGraphicsScene *scene, QUndoCommand *parent) : QUndoCommand(parent), m_item(item), m_scene(scene), m_isRemoved(false) {
  qDebug() << "RemoveCommand: construyendo item: " << m_item;
}

RemoveCommand::~RemoveCommand() {}

void RemoveCommand::undo() {
  if (m_item && m_scene && m_isRemoved) {
    qDebug() << "RemoveCommand: undo item: " << m_item;
    m_scene->addItem(m_item);
    m_isRemoved = false;
  }
}

void RemoveCommand::redo() {
  if (m_item && m_scene && !m_isRemoved) {
    qDebug() << "RemoveCommand: redo item: " << m_item;
    m_item->setSelected(false);
    m_scene->removeItem(m_item);
    m_isRemoved = true;
  }
}
