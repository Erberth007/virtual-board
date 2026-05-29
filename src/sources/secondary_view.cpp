#include "../headers/secondary_view.h"

#include <QGraphicsScene>
#include <QPainter>

SecondaryView::SecondaryView(
  QGraphicsScene* scene,
  QWidget* parent
) : QGraphicsView(scene, parent) {
  setInteractive(false);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setRenderHint(QPainter::Antialiasing);
  setBackgroundBrush(scene->backgroundBrush());
}

void SecondaryView::resizeEvent(QResizeEvent* event) {
  fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
  QGraphicsView::resizeEvent(event);
}