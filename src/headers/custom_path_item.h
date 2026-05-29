#ifndef CUSTOM_PATH_ITEM_H
#define CUSTOM_PATH_ITEM_H

#include <QGraphicsPathItem>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

class CustomPathItem : public QGraphicsPathItem {
  public:
    using QGraphicsPathItem::QGraphicsPathItem;
    virtual ~CustomPathItem() = default;
    QPainterPath shape() const override;
    // void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // CUSTOM_PATH_ITEM_H