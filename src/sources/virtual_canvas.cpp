#include "../headers/virtual_canvas.h"

#include <QMouseEvent>
#include <QGraphicsPathItem>
#include <QUndoStack>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QCursor>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <cmath>
#include <QtMath>

#include "../headers/page.h"
#include "../headers/add_command.h"
#include "../headers/remove_command.h"
#include "../headers/custom_path_item.h"

// ***************** public *****************
VirtualCanvas::VirtualCanvas(QWidget *parent) : QGraphicsView(parent) {
  setupUi();
  updateCursor();
}

// VirtualCanvas::~VirtualCanvas() {
//   if (m_currentPathItem) {
//     delete m_currentPathItem;
//     m_currentPathItem = nullptr;
//   }
// }

void VirtualCanvas::setCurrentPage(Page *page) {
  if (!page) return;
  m_currentPage = page;
  this->setScene(m_currentPage->scene());
}

void VirtualCanvas::setMode(CanvasMode mode) {
  if (m_currentMode != mode) {
    m_lastEraserPos = QPointF();
    clearHoverEffect();
  }
  m_currentMode = mode;

  bool selectionEnabled = (m_currentMode == CanvasMode::Selection);
  updateItemsInteractionFlags(selectionEnabled);

  updateCursor();
  // qDebug() << "EL LIENZO CAMBIÓ AL MODO NUMERAL:" << static_cast<int>(mode);
}

void VirtualCanvas::clearCanvas() {
  if (m_currentPage && m_currentPage->scene() && m_currentPage->undoStack()) {
    m_currentPathItem = nullptr;
    m_currentPage->scene()->clear();
    m_currentPage->undoStack()->clear();
  }
}

void VirtualCanvas::setCurrentColor(const QColor &color) {
  m_currentColor = color;
  updateCursor();
}

void VirtualCanvas::setCurrentLineWidth(int width) {
  m_currentLineWidth = width;
  updateCursor();
}

void VirtualCanvas::pasteFromClipboard(QPointF targetPos) {
  if (!m_currentPage || !m_currentPage->scene()) return;

  clearHoverEffect();
  m_currentPage->scene()->clearSelection();

  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  if (mimeData->hasImage()) {
    QImage image = clipboard->image();
    if (image.isNull()) {
      image = qvariant_cast<QImage>(mimeData->imageData());
    }

    if (!image.isNull()) {
      QPixmap pixmap = QPixmap::fromImage(image);
      QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
      pixmapItem->setZValue(m_nextZValue++);
      
      // Centrar el ítem en la posición targetPos
      pixmapItem->setPos(targetPos);

      pixmapItem->setFlags({});
      // pixmapItem->setFlags(
      //   QGraphicsItem::ItemIsMovable | 
      //   QGraphicsItem::ItemIsSelectable |
      //   QGraphicsItem::ItemSendsGeometryChanges
      // );

      // 2. Dado que m_isAdded está en true, añadimos manualmente a la escena
      m_currentPage->scene()->addItem(pixmapItem);

      // 3. Empujamos al stack (el comando ahora solo se encargará del undo/redo)
      if (m_currentPage->undoStack()) {
        m_currentPage->undoStack()->push(new AddCommand(pixmapItem, m_currentPage->scene()));
      }
    }

    // QPixmap pixmap = qvariant_cast<QPixmap>(mimeData->imageData());
    // if (!pixmap.isNull()) {
    //   QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(pixmap);
      
    //   pixmapItem->setPos(targetPos);

    //   pixmapItem->setFlags(
    //     QGraphicsItem::ItemIsMovable | 
    //     QGraphicsItem::ItemIsSelectable |
    //     QGraphicsItem::ItemSendsGeometryChanges
    //   );

    //   if (m_currentPage->undoStack()) {
    //     m_currentPage->undoStack()->push(new AddCommand(pixmapItem, m_currentPage->scene()));
    //   } else if (m_currentPage->scene()) {
    //     m_currentPage->scene()->addItem(pixmapItem);
    //   } else {
    //     delete pixmapItem;
    //   }
    // }
  }
}

// ***************** protected *****************
void VirtualCanvas::mousePressEvent(QMouseEvent *event) {
  if (!m_currentPage) return;
  if (event->button() != Qt::LeftButton) return;
  
  QPointF scenePos = mapToScene(event->pos());

  switch (m_currentMode) {
    // case CanvasMode::Hand: {
    //   break;
    // }
    // case CanvasMode::Selection: {
    //   QGraphicsView::mousePressEvent(event);
    //   viewport()->update();
    //   break;
    // }
    case CanvasMode::Rectangle:
    case CanvasMode::Ellipse:
    case CanvasMode::Arrow:
    case CanvasMode::Line: {
      m_startPoint = scenePos;
      createDefaultPathItem();
      break;
    }
    case CanvasMode::Pen: {
      m_currentPath = QPainterPath();
      m_currentPath.moveTo(scenePos);
  
      createDefaultPathItem();
      m_currentPathItem->setPath(m_currentPath);
      break;
    }
    // case CanvasMode::Text: {
    //   break;
    // }
    // case CanvasMode::Image: {
    //   break;
    // }
    case CanvasMode::Eraser: {
      m_erasedItems.clear();
      processEraserDetection(scenePos, true);
      break;
    }
    default: {
      QGraphicsView::mousePressEvent(event);
      break;
    }
  }
}

void VirtualCanvas::mouseMoveEvent(QMouseEvent *event) {
  if (!m_currentPage) return;

  bool leftButtonPressed = (event->buttons() & Qt::LeftButton);
  bool isShiftPressed = event->modifiers() & Qt::ShiftModifier;
  QPointF scenePos = mapToScene(event->pos());

  switch (m_currentMode) {
    // case CanvasMode::Hand: {
    //   break;
    // }
    // case CanvasMode::Selection: {
    //   QGraphicsView::mouseMoveEvent(event);
    //   bool isDragging = (event->buttons() & Qt::LeftButton);
    //   if (isDragging) {
    //     if (!scene()->selectedItems().isEmpty()) {
    //       this->setCursor(Qt::SizeAllCursor);
    //     }
    //   } else {
    //     QGraphicsItem *itemUnderMouse = itemAt(event->pos());
    //     if (itemUnderMouse) {
    //       this->setCursor(Qt::SizeAllCursor);
    //     } else {
    //       this->setCursor(Qt::ArrowCursor);
    //     }
    //   }
    //   break;
    // }
    case CanvasMode::Rectangle:
    case CanvasMode::Ellipse:
    case CanvasMode::Arrow:
    case CanvasMode::Line: {
      if (leftButtonPressed && m_currentPathItem && m_currentPathItem->scene()) {
        recalculateCurrentShape(scenePos, isShiftPressed);
      }
      break;
    }
    case CanvasMode::Pen: {
      if (leftButtonPressed && m_currentPathItem && m_currentPathItem->scene()) {
        QPointF lastPoint = m_currentPath.currentPosition();
        QPointF midPoint = (lastPoint + scenePos) / 2.0;
        m_currentPath.quadTo(lastPoint, midPoint);
        m_currentPathItem->setPath(m_currentPath);
      }
      break;
    }
    // case CanvasMode::Text: {
    //   break;
    // }
    // case CanvasMode::Image: {
    //   break;
    // }
    case CanvasMode::Eraser: {
      processEraserDetection(scenePos, leftButtonPressed);
      break;
    }
    default: {
      clearHoverEffect();
      QGraphicsView::mouseMoveEvent(event);
      break;
    }
  }
}

void VirtualCanvas::mouseReleaseEvent(QMouseEvent *event) {
  if (!m_currentPage) return;
  if (event->button() != Qt::LeftButton) return;

  // if (m_currentPage->undoStack()->index() == 0) {
  //   m_currentPage->undoStack()->clear();
  // }

  switch (m_currentMode) {
    // case CanvasMode::Hand: {
    //   break;
    // }
    // case CanvasMode::Selection: {
    //   QGraphicsView::mouseReleaseEvent(event);
    //   viewport()->update();
    //   break;
    // }
    case CanvasMode::Rectangle:
    case CanvasMode::Ellipse:
    case CanvasMode::Arrow:
    case CanvasMode::Line:
    case CanvasMode::Pen: {
      if (m_currentPathItem && m_currentPage->undoStack()) {
        m_currentPage->undoStack()->push(new AddCommand(m_currentPathItem, m_currentPage->scene()));
      }
      qDebug() << "Item creado...";
      m_currentPathItem = nullptr;
      break;
    }
    // case CanvasMode::Text: {
    //   break;
    // }
    // case CanvasMode::Image: {
    //   break;
    // }
    case CanvasMode::Eraser: {
      m_erasedItems.clear();
      break;
    }
    default: {
      QGraphicsView::mouseReleaseEvent(event);
      break;
    }
  }
}

void VirtualCanvas::keyPressEvent(QKeyEvent *event) {
  // !event->isAutoRepeat()

  // check for special angles: Shift
  if (event->key() == Qt::Key_Shift) {
    if (m_currentPathItem && (m_currentMode == CanvasMode::Rectangle || m_currentMode == CanvasMode::Ellipse)) {
      QPointF scenePos = mapToScene(viewport()->mapFromGlobal(QCursor::pos()));
      recalculateCurrentShape(scenePos, true);
    }
  }
  // check for delete object: Delete / Backspace
  else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
    QList<QGraphicsItem*> selected = scene()->selectedItems();
    if (selected.isEmpty()) return;
    for (QGraphicsItem *item : selected) {
      item->setSelected(false);
      m_currentPage->undoStack()->push(new RemoveCommand(item, m_currentPage->scene()));
    }
  }
  // check for paste from clipboard: Ctrl + V
  else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
    QPoint mousePosViewport = viewport()->mapFromGlobal(QCursor::pos());
    QPointF scenePos = mapToScene(mousePosViewport);
    pasteFromClipboard(scenePos);
  }
  else if (event->key() == Qt::Key_F && !event->isAutoRepeat()) {
    m_isTemporaryCursorActive = true;
    updateCursor();
  }
  else {
    QGraphicsView::keyPressEvent(event);
  }
}

void VirtualCanvas::keyReleaseEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Shift) {
    if (m_currentPathItem && (m_currentMode == CanvasMode::Rectangle || m_currentMode == CanvasMode::Ellipse)) {
      QPointF scenePos = mapToScene(viewport()->mapFromGlobal(QCursor::pos()));
      recalculateCurrentShape(scenePos, false);
    }
  }
  else if (event->key() == Qt::Key_F && !event->isAutoRepeat()) {
    m_isTemporaryCursorActive = false;
    updateCursor();
  }
  else {
    QGraphicsView::keyReleaseEvent(event);
  }
}

// ***************** private *****************
void VirtualCanvas::setupUi() {
  setRenderHint(QPainter::Antialiasing);
  setRenderHint(QPainter::SmoothPixmapTransform);
  
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  setAlignment(Qt::AlignLeft | Qt::AlignTop);
  setFrameStyle(QFrame::NoFrame);

  setFocusPolicy(Qt::StrongFocus);

  setMouseTracking(true);
  m_eraserStroker.setCapStyle(Qt::RoundCap);
  m_eraserStroker.setJoinStyle(Qt::RoundJoin);
  m_eraserStroker.setCurveThreshold(0.1);
}

void VirtualCanvas::updateCursor() {
  int pixmapSize = 64;
  QPixmap pixmap(pixmapSize, pixmapSize);
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  int center = pixmapSize / 2;
  int radius = m_currentLineWidth / 2;

  QCursor currentCursor;

  if (m_isTemporaryCursorActive) {
    painter.setPen(Qt::NoPen);

    QColor outer(255, 0, 0, 40);
    painter.setBrush(outer);
    int outerRadius = 15;
    painter.drawEllipse(
      pixmapSize/2 - outerRadius,
      pixmapSize/2 - outerRadius,
      outerRadius * 2,
      outerRadius * 2
    );

    QColor inner(255, 0, 0, 180);
    painter.setBrush(inner);
    int innerRadius = 2 * outerRadius / 3;
    painter.drawEllipse(
      pixmapSize/2 - innerRadius,
      pixmapSize/2 - innerRadius,
      innerRadius * 2,
      innerRadius * 2
    );

    currentCursor = QCursor(pixmap, center, center);
  } else {
    painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
    painter.drawLine(center, 0, center, pixmapSize);
    painter.drawLine(0, center, pixmapSize, center);

    painter.setPen(QPen(m_currentColor, 2));
    painter.drawEllipse(center - radius, center - radius, radius * 2, radius * 2);
  
    switch (m_currentMode) {
      case CanvasMode::Hand: {
        currentCursor = Qt::OpenHandCursor;
        break;
      }
      case CanvasMode::Selection: {
        currentCursor = Qt::ArrowCursor;
        break;
      }
      case CanvasMode::Rectangle: {
        int startX = center + radius + 10;
        int startY = center + radius + 10;
        painter.setBrush(m_currentColor);
        painter.drawRect(startX, startY, 12, 12);
        currentCursor = QCursor(pixmap, center, center);
        break;
      }
      case CanvasMode::Ellipse: {
        int startX = center + radius + 10;
        int startY = center + radius + 10;
        painter.setBrush(m_currentColor);
        painter.drawEllipse(startX, startY, 12, 12);
        currentCursor = QCursor(pixmap, center, center);
        break;
      }
      case CanvasMode::Arrow: {
        int startX = center + radius + 10;
        int startY = center + radius + 16;
  
        painter.setPen(QPen(m_currentColor, 3));
        painter.drawLine(startX, startY, startX + 12, startY);
        painter.drawLine(startX + 12, startY, startX + 8, startY - 5);
        painter.drawLine(startX + 12, startY, startX + 8, startY + 5);
        currentCursor = QCursor(pixmap, center, center);
        break;
      }
      case CanvasMode::Line: {
        int startX = center + radius + 10;
        int startY = center + radius + 16;
  
        painter.setPen(QPen(m_currentColor, 3));
        painter.drawLine(startX, startY, startX + 12, startY);
        currentCursor = QCursor(pixmap, center, center);
        break;
      }
      case CanvasMode::Pen: {
        int startX = center + radius + 10;
        int startY = center + radius + 16;
  
        QPainterPath curveIcon;
        curveIcon.moveTo(startX, startY);
        curveIcon.cubicTo(
          startX + 4, startY - 12,
          startX + 8, startY + 12,
          startX + 12, startY
        );
        
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(m_currentColor, 3));
        painter.drawPath(curveIcon);
        currentCursor = QCursor(pixmap, center, center);
        break;
      }
      case CanvasMode::Text: {
        currentCursor = Qt::IBeamCursor;
        break;
      }
      // case CanvasMode::Image: {
      //   currentCursor = Qt::ArrowCursor;
      //   break;
      // }
      case CanvasMode::Eraser: {
        int startX = center + radius + 10;
        int startY = center + radius + 10;
  
        painter.setPen(QPen(Qt::red, 3));
        painter.drawEllipse(startX, startY, 12, 12);
        currentCursor = QCursor(pixmap, center, center);
        break;
      }
      default: {
        currentCursor = Qt::ArrowCursor;
        break;
      }
    }
  }

  

  painter.end();
  setCursor(currentCursor);
}

// void VirtualCanvas::updateCursor() {
//   int size = m_currentLineWidth + 4 < 8 ? 8 : m_currentLineWidth + 4;

//   QPixmap pixmap(size, size);
//   pixmap.fill(Qt::transparent);

//   QPainter painter(&pixmap);
//   painter.setRenderHint(QPainter::Antialiasing);

//   QCursor currentCursor;

//   switch (m_currentMode) {
//     case CanvasMode::Hand: {
//       currentCursor = Qt::OpenHandCursor;
//       break;
//     }
//     case CanvasMode::Selection: {
//       currentCursor = Qt::ArrowCursor;
//       // currentCursor = Qt::SizeAllCursor;
//       // currentCursor = Qt::SizeVerCursor;
//       break;
//     }
//     case CanvasMode::Rectangle:
//     case CanvasMode::Ellipse: {
//       currentCursor = Qt::CrossCursor;
//       break;
//     }
//     // case CanvasMode::Arrow:
//     //   currentCursor = Qt::ArrowCursor;
//     //   break;
//     // case CanvasMode::Line:
//     //   currentCursor = Qt::ArrowCursor;
//     //   break;
//     case CanvasMode::Pen: {
//       painter.setPen(QPen(Qt::black, 1));
//       painter.setBrush(QBrush(m_currentColor));
//       painter.drawEllipse(1, 1, size - 2, size - 2);
//       currentCursor = QCursor(pixmap, size/2, size/2);
//       break;
//     }
//     case CanvasMode::Text: {
//       currentCursor = Qt::IBeamCursor;
//       break;
//     }
//     // case CanvasMode::Image: {
//     //   currentCursor = Qt::ArrowCursor;
//     //   break;
//     // }
//     case CanvasMode::Eraser: {
//       painter.setPen(QPen(Qt::red, 2));
//       painter.setBrush(Qt::NoBrush);
//       painter.drawEllipse(1, 1, size - 2, size - 2);
//       currentCursor = QCursor(pixmap, size/2, size/2);
//       break;
//     }
//     default: {
//       currentCursor = Qt::ArrowCursor;
//       break;
//     }
//   }

//   painter.end();
//   setCursor(currentCursor);
// }

void VirtualCanvas::processEraserDetection(const QPointF &scenePos, bool isLeftButtonPressed) {
  if (!m_currentPage || !m_currentPage->scene() || !m_currentPage->undoStack()) return;
  
  if (m_lastEraserPos.isNull()) m_lastEraserPos = scenePos;
  if (!isLeftButtonPressed && QLineF(m_lastEraserPos, scenePos).length() < 1.5) return;

  int currentEraserSize = qMax(8, m_currentLineWidth + 4);
  qreal eraserRadius = currentEraserSize / 2.0;
  QRectF detectionRect = QRectF(m_lastEraserPos, scenePos).normalized().adjusted(-eraserRadius, -eraserRadius, eraserRadius, eraserRadius);

  QGraphicsItem *genericItemUnderMouse = nullptr;
  QList<QGraphicsItem *> candidates = m_currentPage->scene()->items(detectionRect);

  QPainterPath mouseTrajectory;
  mouseTrajectory.moveTo(m_lastEraserPos);
  mouseTrajectory.lineTo(scenePos);

  for (QGraphicsItem *item : candidates) {
    bool isHit = false;
    qreal itemWidth = 1.0;

    QPainterPath localTrajectory = item->sceneTransform().inverted().map(mouseTrajectory);
    QPointF localScenePos = item->mapFromScene(scenePos);

    if (auto pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(item)) {
      itemWidth = pathItem->pen().widthF();
      m_eraserStroker.setWidth(currentEraserSize + itemWidth);

      QPainterPath strokePath = m_eraserStroker.createStroke(pathItem->path());
      isHit = strokePath.intersects(localTrajectory) || strokePath.contains(localScenePos);
    }
    else if (auto lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(item)) {
      QPainterPath linePath;
      linePath.moveTo(lineItem->line().p1());
      linePath.lineTo(lineItem->line().p2());

      itemWidth = lineItem->pen().widthF();
      m_eraserStroker.setWidth(currentEraserSize + itemWidth);
      
      QPainterPath strokePath = m_eraserStroker.createStroke(linePath);
      isHit = strokePath.intersects(localTrajectory) || strokePath.contains(localScenePos);
    }
    // else if (auto pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
    //   QRectF imageRect = pixmapItem->boundingRect();
    //   isHit = localTrajectory.intersects(imageRect) || imageRect.contains(localScenePos);
    // }

    if (isHit) {
      genericItemUnderMouse = item;
      break;
    }
  }

  if (isLeftButtonPressed) {
    if (genericItemUnderMouse && !m_erasedItems.contains(genericItemUnderMouse)) {
      m_erasedItems.insert(genericItemUnderMouse);
      if (genericItemUnderMouse == m_hoveredItem) {
        m_hoveredItem->setOpacity(1.0);
        m_hoveredItem = nullptr;
      }
      m_currentPage->undoStack()->push(new RemoveCommand(genericItemUnderMouse, m_currentPage->scene()));
      qDebug() << "Item borrado ...";
    }
  } else {
    if (genericItemUnderMouse != m_hoveredItem) {
      clearHoverEffect();
      if (genericItemUnderMouse) {
        genericItemUnderMouse->setOpacity(0.5);
        m_hoveredItem = genericItemUnderMouse;
      }
    }
  }

  m_lastEraserPos = scenePos;
}

void VirtualCanvas::clearHoverEffect() {
  if (m_hoveredItem) {
    m_hoveredItem->setOpacity(1.0);
    m_hoveredItem = nullptr;
  }
}

void VirtualCanvas::createDefaultPathItem() {
  m_currentPathItem = new CustomPathItem();
  m_currentPathItem->setZValue(m_nextZValue++);
  m_currentPathItem->setFlags({});
  // m_currentPathItem->setFlags(
  //   QGraphicsItem::ItemIsMovable | 
  //   QGraphicsItem::ItemIsSelectable | 
  //   QGraphicsItem::ItemSendsGeometryChanges
  // );
  QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  m_currentPathItem->setPen(pen);
  m_currentPage->scene()->addItem(m_currentPathItem);
}

QPointF VirtualCanvas::calculateSnappedPoint(const QPointF &start, const QPointF &end) {
  QLineF line(start, end);
  qreal angle = line.angle(); // return angle in grads (0-360)
  qreal length = line.length();

  qreal minimumAngle = 15.0;
  qreal snappedAngle = qRound(angle / minimumAngle) * minimumAngle;

  // new line with adjusted angle
  QLineF snappedLine;
  snappedLine.setP1(start);
  snappedLine.setAngle(snappedAngle);
  snappedLine.setLength(length);

  return snappedLine.p2();
}

void VirtualCanvas::recalculateCurrentShape(QPointF scenePos, bool isShiftPressed) {
  if (!m_currentPathItem) return;

  QRectF rect(m_startPoint, scenePos);
  
  if (isShiftPressed) {
    qreal side = qMax(qAbs(rect.width()), qAbs(rect.height()));
    rect.setWidth(side * (rect.width() >= 0 ? 1 : -1));
    rect.setHeight(side * (rect.height() >= 0 ? 1 : -1));
  }

  QPainterPath path;
  switch (m_currentMode) {
    case CanvasMode::Rectangle: {
      path.addRect(rect.normalized());
      break;
    }
    case CanvasMode::Ellipse: {
      path.addEllipse(rect.normalized());
      break;
    }
    case CanvasMode::Arrow:{
      QPointF snappedEnd = isShiftPressed ? calculateSnappedPoint(m_startPoint, scenePos) : scenePos;
      path.moveTo(m_startPoint);
      path.lineTo(snappedEnd);

      double angle = std::atan2(m_startPoint.y() - snappedEnd.y(), m_startPoint.x() - snappedEnd.x());
      double arrowSize = qMax(10.0, m_currentLineWidth * 2.5);
      double spreadDegrees = qMin(45.0, 20.0 + m_currentLineWidth * 1.5);
      double spreadRadians = qDegreesToRadians(spreadDegrees);
      
      QPointF p1 = snappedEnd + QPointF(std::cos(angle + spreadRadians) * arrowSize, std::sin(angle + spreadRadians) * arrowSize);
      QPointF p2 = snappedEnd + QPointF(std::cos(angle - spreadRadians) * arrowSize, std::sin(angle - spreadRadians) * arrowSize);
      
      path.moveTo(p1);
      path.lineTo(snappedEnd);
      path.lineTo(p2);
      break;
    }
    case CanvasMode::Line: {
      QPointF snappedEnd = isShiftPressed ? calculateSnappedPoint(m_startPoint, scenePos) : scenePos;
      path.moveTo(m_startPoint);
      path.lineTo(snappedEnd);
      break;
    }
    default:
      break;
  }

  m_currentPathItem->setPath(path);
}

void VirtualCanvas::updateItemsInteractionFlags(bool enabled) {
  if (!m_currentPage || !m_currentPage->scene()) return;

  QGraphicsItem::GraphicsItemFlags flags;

  if (enabled) {
    flags =
      QGraphicsItem::ItemIsMovable |
      QGraphicsItem::ItemIsSelectable;
      // QGraphicsItem::ItemSendsGeometryChanges;
  }

  for (QGraphicsItem *item : m_currentPage->scene()->items()) {
    item->setFlags(flags);
  }
}