#ifndef SECONDARY_VIEW_H
#define SECONDARY_VIEW_H

#include <QGraphicsView>

class QGraphicsScene;
class QResizeEvent;

class SecondaryView : public QGraphicsView {
  public:
    explicit SecondaryView(
      QGraphicsScene *scene,
      QWidget *parent = nullptr
    );

  protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // SECONDARY_VIEW_H
