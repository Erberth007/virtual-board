#ifndef ADD_COMMAND_H
#define ADD_COMMAND_H

#include <QUndoCommand>
#include <QGraphicsScene>
#include <QGraphicsItem>

class AddCommand : public QUndoCommand {
  public:
    AddCommand(QGraphicsItem *item, QGraphicsScene *scene, QUndoCommand *parent = nullptr);
    ~AddCommand();

    void undo() override;
    void redo() override;

  private:
    QGraphicsItem *m_item;
    QGraphicsScene *m_scene;
    bool m_isAdded;
};

#endif // ADD_COMMAND_H