#ifndef PAGE_H
#define PAGE_H

#include <QGraphicsScene>
#include <QUndoStack>
#include <QObject>

class Page : public QObject {
  Q_OBJECT

  public:
    explicit Page(QObject *parent = nullptr);
    ~Page() = default;

    QGraphicsScene* scene() const { return m_scene; }
    QUndoStack* undoStack() const { return m_undoStack; }

    // En el futuro, aquí podemos meter métodos de persistencia por página:
    // void saveToFile(const QString &path);
    // void loadFromFile(const QString &path);

  private:
    QGraphicsScene *m_scene = nullptr;
    QUndoStack *m_undoStack = nullptr;
};

#endif // PAGE_H