#include "../headers/add_command.h"
#include <QDebug>

AddCommand::AddCommand(QGraphicsItem *item, QGraphicsScene *scene, QUndoCommand *parent) : QUndoCommand(parent), m_item(item), m_scene(scene), m_isAdded(true) {
  qDebug() << "AddCommand: construyendo item: " << m_item;
}

AddCommand::~AddCommand() {}

void AddCommand::undo() {
  if (m_item && m_scene && m_isAdded) {
    qDebug() << "AddCommand: undo item: " << m_item;
    m_item->setSelected(false);
    m_scene->removeItem(m_item);
    m_isAdded = false;
  }
}

void AddCommand::redo() {
  if (m_item && m_scene && !m_isAdded) {
    qDebug() << "AddCommand: redo item: " << m_item;
    m_scene->addItem(m_item);
    m_isAdded = true;
  }
}
