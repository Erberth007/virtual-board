#include "../headers/custom_path_item.h"
#include <QPainterPathStroker>

QPainterPath CustomPathItem::shape() const {
  QPainterPathStroker stroker;
  stroker.setWidth(this->pen().widthF() + 6.0); 
  stroker.setCapStyle(this->pen().capStyle());
  stroker.setJoinStyle(this->pen().joinStyle());
  return stroker.createStroke(this->path()); 
}

// void CustomPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//   // 1. Draw the native path first (your actual freehand drawings)
//   QGraphicsPathItem::paint(painter, option, widget);
//   // 2. If the item is currently selected, draw the custom bounding box
//   if (this->isSelected()) {
//     painter->save(); // Save the current painter state
//     // Configure an elegant bounding box style (Excalidraw/Figma style)
//     QPen selectionPen;
//     selectionPen.setColor(QColor(0, 120, 215)); // Nice accent blue
//     selectionPen.setWidthF(1.5);
//     selectionPen.setStyle(Qt::DashLine);       // Dashed/Dotted line
//     painter->setPen(selectionPen);
//     painter->setBrush(Qt::NoBrush);             // Keep the inside hollow

//     // Get the bounding rectangle of the item and add a small padding (3 pixels)
//     qreal margin = -5;
//     QRectF rect = this->boundingRect();
//     rect.adjust(-margin, -margin, margin, margin);

//     // Draw the dashed rectangle around the drawing
//     painter->drawRect(rect);

//     // Optional: Draw small solid squares on the corners for a nicer look
//     painter->setBrush(QBrush(QColor(0, 120, 215)));
//     selectionPen.setStyle(Qt::SolidLine);
//     painter->setPen(selectionPen);
    
//     qreal handleSize = 5.0;
//     painter->drawRect(rect.topLeft().x() - handleSize/2, rect.topLeft().y() - handleSize/2, handleSize, handleSize);
//     painter->drawRect(rect.topRight().x() - handleSize/2, rect.topRight().y() - handleSize/2, handleSize, handleSize);
//     painter->drawRect(rect.bottomLeft().x() - handleSize/2, rect.bottomLeft().y() - handleSize/2, handleSize, handleSize);
//     painter->drawRect(rect.bottomRight().x() - handleSize/2, rect.bottomRight().y() - handleSize/2, handleSize, handleSize);

//     painter->restore(); // Restore painter state
//   }
// }