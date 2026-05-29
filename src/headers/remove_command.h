#ifndef REMOVE_COMMAND_H
#define REMOVE_COMMAND_H

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>

class RemoveCommand : public QUndoCommand {
  public:
    RemoveCommand(QGraphicsItem *item, QGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~RemoveCommand();

    void undo() override;
    void redo() override;

  private:
    QGraphicsItem *m_item;
    QGraphicsScene *m_scene;
    bool m_isRemoved;
};

#endif // REMOVE_COMMAND_H