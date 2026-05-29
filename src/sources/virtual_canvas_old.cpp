#include "../headers/virtual_canvas.h"

#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include "../headers/customPathItem.h"
#include <cmath>

// ***************** public *****************
VirtualCanvas::VirtualCanvas(QWidget *parent) : QGraphicsView(parent) {
  setupScene();
  // setupGridTexture();
  updateCursor();

  setMouseTracking(true);
}

void VirtualCanvas::setMode(CanvasMode mode) {
  if (m_currentMode == mode) return;
  m_currentMode = mode;

  // Clear any active selection in the scene immediately when changing tools
  if (scene()) {
    scene()->clearSelection();
  }

  bool isSelectionMode = (mode == CanvasMode::Selection);
  for (QGraphicsItem *item : scene()->items()) {
    item->setAcceptHoverEvents(isSelectionMode);
    // Quitamos el setAcceptedMouseButtons que rompía el flujo
  }

  updateCursor();
  this->viewport()->update(); // Force visual refresh to clear default Qt boxes
  // Podemos abortar dibujos en curso si cambiamos de modo a mitad de un trazo
  //m_tempLine = nullptr;
  //qDebug() << "Modo del lienzo actualizado a:" << static_cast<int>(m_currentMode);
}

void VirtualCanvas::setCurrentColor(const QColor &color) {
  m_currentColor = color;
  updateCursor();
}

QColor VirtualCanvas::currentColor() const { return m_currentColor; }

void VirtualCanvas::setCurrentLineWidth(int width) {
  m_currentLineWidth = width;
  updateCursor();
}

int VirtualCanvas::currentLineWidth() const { return m_currentLineWidth; }

void VirtualCanvas::clearCanvas() {
  drawingScene->clear();
}

void VirtualCanvas::deleteSelectedItems() {
  QList<QGraphicsItem*> selected = scene()->selectedItems();
  if (selected.isEmpty()) return;
  for (QGraphicsItem* item : selected) {
    scene()->removeItem(item);
    delete item;
  }
}

// ***************** protected *****************
void VirtualCanvas::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);
  this->scene()->setSceneRect(this->rect());
}

void VirtualCanvas::mousePressEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton) return;

  QPointF scenePos = mapToScene(event->pos());

  switch (m_currentMode) {
    // case CanvasMode::Hand: {
    //   break;
    // }
    case CanvasMode::Selection: {
      QGraphicsView::mousePressEvent(event);
      viewport()->update();
      break;
    }
    case CanvasMode::Rectangle: {
      m_startPoint = scenePos;
      QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      // m_currentRectItem = scene()->addRect(QRectF(m_startPoint, QSizeF(0, 0)), pen);
      CustomPathItem *customRect = new CustomPathItem();
      customRect->setPen(pen);
      scene()->addItem(customRect);
      m_currentPathItem = customRect;
      break;
    }
    case CanvasMode::Ellipse: {
      m_startPoint = scenePos;
      QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      // m_currentEllipseItem = scene()->addEllipse(QRectF(m_startPoint, QSizeF(0, 0)), pen);
      CustomPathItem *customEllipse = new CustomPathItem();
      customEllipse->setPen(pen);
      scene()->addItem(customEllipse);
      m_currentPathItem = customEllipse;
      break;
    }
    case CanvasMode::Arrow: {
      m_startPoint = scenePos;
      QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      m_currentArrowLineItem = scene()->addLine(QLineF(m_startPoint, m_startPoint), pen);
      m_currentArrowHeadItem = scene()->addPolygon(QPolygonF(), pen, QBrush(m_currentColor));
      break;
    }
    case CanvasMode::Line: {
      m_startPoint = scenePos;
      QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      m_currentLineItem = scene()->addLine(QLineF(m_startPoint, m_startPoint), pen);
      break;
    }
    case CanvasMode::Pen: {
      m_currentPath = QPainterPath();
      m_currentPath.moveTo(scenePos);

      CustomPathItem *customItem = new CustomPathItem();
      QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      customItem->setPen(pen);
      customItem->setPath(m_currentPath);
      
      scene()->addItem(customItem);
      m_currentPathItem = customItem; // m_currentPathItem debe ser QGraphicsPathItem*

      // ***************** old code *****************
      // m_currentPath = QPainterPath();
      // // m_currentPath.setFillRule(Qt::WindingFill);
      // m_currentPath.moveTo(scenePos);
      // QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      // m_currentPathItem = scene()->addPath(m_currentPath, pen);
      
      // ***************** old code *****************
      // m_currentPathItem = new QGraphicsPathItem();
      // QPen pen(m_currentColor, m_currentLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
      // m_currentPathItem->setPen(pen);
      // scene()->addItem(m_currentPathItem);
      break;
    }
    // case CanvasMode::Text: {
    //   break;
    // }
    // case CanvasMode::Image: {
    //   break;
    // }
    case CanvasMode::Eraser: {
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
  // if (!(event->buttons() & Qt::LeftButton)) return;
  bool leftButtonPressed = (event->buttons() & Qt::LeftButton);
  QPointF scenePos = mapToScene(event->pos());

  switch (m_currentMode) {
    // case CanvasMode::Hand: {
    //   break;
    // }
    case CanvasMode::Selection: {
      QGraphicsView::mouseMoveEvent(event);
      bool isDragging = (event->buttons() & Qt::LeftButton);
      if (isDragging) {
        if (!scene()->selectedItems().isEmpty()) {
          this->setCursor(Qt::SizeAllCursor);
        }
      } else {
        QGraphicsItem *itemUnderMouse = itemAt(event->pos());
        if (itemUnderMouse) {
          this->setCursor(Qt::SizeAllCursor);
        } else {
          this->setCursor(Qt::ArrowCursor);
        }
      }
      break;
    }
    case CanvasMode::Rectangle: {
      // if (m_currentRectItem) {
      //   QRectF rect(m_startPoint, scenePos);
      //   m_currentRectItem->setRect(rect.normalized()); // normalized() prevents crash if drawing backwards
      // }
      if (m_currentPathItem) {
        QRectF rect(m_startPoint, scenePos);
        QRectF normalizedRect = rect.normalized();
        QPainterPath rectPath;
        rectPath.addRect(normalizedRect); // Build a rectangle vector path
        m_currentPathItem->setPath(rectPath);
      }
      break;
    }
    case CanvasMode::Ellipse: {
      // if (m_currentEllipseItem) {
      //   QRectF rect(m_startPoint, scenePos);
      //   m_currentEllipseItem->setRect(rect.normalized());
      // }
      if (m_currentPathItem) {
        QRectF rect(m_startPoint, scenePos);
        QRectF normalizedRect = rect.normalized();
        QPainterPath ellipsePath;
        ellipsePath.addEllipse(normalizedRect); // Build an ellipse vector path
        m_currentPathItem->setPath(ellipsePath);
      }
      break;
    }
    case CanvasMode::Arrow: {
      if (m_currentArrowLineItem && m_currentArrowHeadItem) {
        QLineF line(m_startPoint, scenePos);
        m_currentArrowLineItem->setLine(line);

        // Calculate arrow head coordinates (triangle tip)
        if (line.length() > 0) {
          qreal arrowSize = m_currentLineWidth * 3.0 + 8.0; // Dynamic size based on line width
          double angle = std::atan2(-line.dy(), line.dx());

          QPointF arrowP1 = line.p2() - QPointF(std::cos(angle + M_PI / 6) * arrowSize, -std::sin(angle + M_PI / 6) * arrowSize);
          QPointF arrowP2 = line.p2() - QPointF(std::cos(angle - M_PI / 6) * arrowSize, -std::sin(angle - M_PI / 6) * arrowSize);

          QPolygonF arrowHead;
          arrowHead << line.p2() << arrowP1 << arrowP2;
          m_currentArrowHeadItem->setPolygon(arrowHead);
        }
      }
      break;
    }
    case CanvasMode::Line: {
      if (m_currentLineItem) {
        QPointF endPoint = scenePos;
        if (event->modifiers() & Qt::ShiftModifier) {
          endPoint = calculateSnappedPoint(m_startPoint, endPoint);
        }
        m_currentLineItem->setLine(QLineF(m_startPoint, endPoint));
      }
      break;
    }
    case CanvasMode::Pen: {
      // if (leftButtonPressed && m_currentPathItem) {
      //   m_currentPath.lineTo(scenePos);
      //   m_currentPathItem->setPath(m_currentPath);
      // }
      if (leftButtonPressed && m_currentPathItem) {
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
  if (event->button() == Qt::LeftButton) {
    switch (m_currentMode) {
      // case CanvasMode::Hand: {
      //   break;
      // }
      case CanvasMode::Selection: {
        QGraphicsView::mouseReleaseEvent(event);
        viewport()->update();
        break;
      }
      // case CanvasMode::Rectangle: {
      //   if (m_currentRectItem) {
      //     m_currentRectItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
      //     m_currentRectItem->setFlag(QGraphicsItem::ItemIsMovable, true);
      //   }
      //   m_currentRectItem = nullptr;
      //   break;
      // }
      // case CanvasMode::Ellipse: {
      //   if (m_currentEllipseItem) {
      //     m_currentEllipseItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
      //     m_currentEllipseItem->setFlag(QGraphicsItem::ItemIsMovable, true);
      //   }
      //   m_currentEllipseItem = nullptr;
      //   break;
      // }
      case CanvasMode::Rectangle:
      case CanvasMode::Ellipse: {
        if (m_currentPathItem) {
            m_currentPathItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
            m_currentPathItem->setFlag(QGraphicsItem::ItemIsMovable, true);
            scene()->removeItem(m_currentPathItem);
            m_undoStack->push(new AddCommand(m_currentPathItem, scene()));
        }
        m_currentPathItem = nullptr;
        break;
      }
      case CanvasMode::Arrow: {
        if (m_currentArrowLineItem && m_currentArrowHeadItem) {
          // Group the line and the head so they select and move together as a single item
          QGraphicsItemGroup* arrowGroup = scene()->createItemGroup({m_currentArrowLineItem, m_currentArrowHeadItem});
          arrowGroup->setFlag(QGraphicsItem::ItemIsSelectable, true);
          arrowGroup->setFlag(QGraphicsItem::ItemIsMovable, true);
          scene()->removeItem(arrowGroup);
          m_undoStack->push(new AddCommand(arrowGroup, scene()));
        }
        m_currentArrowLineItem = nullptr;
        m_currentArrowHeadItem = nullptr;
        break;
      }
      case CanvasMode::Line: {
        if (m_currentLineItem) {
          m_currentLineItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
          m_currentLineItem->setFlag(QGraphicsItem::ItemIsMovable, true);
          scene()->removeItem(m_currentLineItem);
          m_undoStack->push(new AddCommand(m_currentLineItem, scene()));
        }
        m_currentLineItem = nullptr;
        break;
      }
      case CanvasMode::Pen: {
        if (m_currentPathItem) {
          m_currentPathItem->setFlag(QGraphicsItem::ItemIsSelectable, true);
          m_currentPathItem->setFlag(QGraphicsItem::ItemIsMovable, true);
          scene()->removeItem(m_currentPathItem);
          m_undoStack->push(new AddCommand(m_currentPathItem, scene()));
        }
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
        m_lastEraserPos = QPointF();
        processEraserDetection(mapToScene(event->pos()), false);
        break;
      }
      default: {
        break;
      }
    }
  }
  QGraphicsView::mouseReleaseEvent(event);
}

void VirtualCanvas::leaveEvent(QEvent *event) {
  clearHoverEffect();
  QGraphicsView::leaveEvent(event);
}

void VirtualCanvas::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
    QPoint mousePosViewport = viewport()->mapFromGlobal(QCursor::pos());
    QPointF scenePos = mapToScene(mousePosViewport);
    pasteFromClipboard(scenePos);
  }
  // delete logic (Delete / Backspace)
  else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
    deleteSelectedItems();
    event->accept();
  }
  // Check for Undo: Ctrl + Z
  else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Z) {
    if (m_undoStack->canUndo()) {
      m_undoStack->undo();
      this->viewport()->update(); // Force immediate visual refresh
    }
    event->accept();
  }
  // Check for Redo: Ctrl + Y
  else if ((event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_Y) {
    if (m_undoStack->canRedo()) {
      m_undoStack->redo();
      this->viewport()->update(); // Force immediate visual refresh
    }
    event->accept();
  }
  else {
    QGraphicsView::keyPressEvent(event);
  }
}

// ***************** private *****************
void VirtualCanvas::setupScene() {
  // this->setFocusPolicy(Qt::StrongFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setFrameStyle(QFrame::NoFrame);

  // Setup the scene
  drawingScene = new QGraphicsScene(this);
  setScene(drawingScene);

  // Setup viewport properties for smooth drawing
  setRenderHint(QPainter::Antialiasing);
  setRenderHint(QPainter::SmoothPixmapTransform);
  setRenderHint(QPainter::HighQualityAntialiasing);
  setOptimizationFlags(QGraphicsView::DontSavePainterState);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  m_undoStack = new QUndoStack(this);
  m_undoStack->setUndoLimit(100);
}

void VirtualCanvas::setupGridTexture() {
  int gridSize = 50;
  QPixmap gridPixmap(gridSize, gridSize);
  gridPixmap.fill(Qt::white);
  QPainter painter(&gridPixmap);
  painter.setPen(QPen(QColor(230, 230, 230), 1)); // Light gray for grid
  painter.drawRect(0, 0, gridSize, gridSize);
  drawingScene->setBackgroundBrush(gridPixmap);
  /*int gridSize = 50;
  QPixmap gridPixmap(gridSize, gridSize);
  gridPixmap.fill(Qt::white);
  QPainter painter(&gridPixmap);
  painter.setPen(QPen(QColor(220, 220, 220))); // Light gray for grid
  painter.drawLine(0, 0, gridSize, 0);
  painter.drawLine(0, 0, 0, gridSize);
  drawingScene->setBackgroundBrush(gridPixmap);*/
}

void VirtualCanvas::updateCursor() {
  int size = m_currentLineWidth + 4;
  if (size < 8) size = 8;
  QPixmap pixmap(size, size);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);
  QCursor currentCursor;

  switch (m_currentMode) {
    case CanvasMode::Hand: {
      currentCursor = Qt::OpenHandCursor;
      break;
    }
    case CanvasMode::Selection: {
      currentCursor = Qt::ArrowCursor;
      // currentCursor = Qt::SizeAllCursor;
      // currentCursor = Qt::SizeVerCursor;
      break;
    }
    case CanvasMode::Rectangle:
    case CanvasMode::Ellipse: {
      currentCursor = Qt::CrossCursor;
      break;
    }
    // case CanvasMode::Arrow:
    //   currentCursor = Qt::ArrowCursor;
    //   break;
    // case CanvasMode::Line:
    //   currentCursor = Qt::ArrowCursor;
    //   break;
    case CanvasMode::Pen: {
      painter.setPen(QPen(Qt::black, 1));
      painter.setBrush(QBrush(m_currentColor));
      painter.drawEllipse(1, 1, size - 2, size - 2);
      currentCursor = QCursor(pixmap, size/2, size/2);
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
      //setCursor(Qt::CrossCursor);
      // ****************************************
      //painter.setPen(QPen(Qt::red, 2));
      //painter.setBrush(Qt::NoBrush);
      //painter.drawRect(1, 1, size - 2, size - 2);
      //painter.setPen(QPen(Qt::red, 1, Qt::DashLine)); // Línea punteada roja
      painter.setPen(QPen(Qt::red, 2));
      painter.setBrush(Qt::NoBrush);
      painter.drawEllipse(1, 1, size - 2, size - 2);
      currentCursor = QCursor(pixmap, size/2, size/2);
      break;
    }
    default: {
      currentCursor = Qt::ArrowCursor;
      break;
    }
  }

  painter.end();
  this->setCursor(currentCursor);
}

void VirtualCanvas::processEraserDetection(const QPointF &currentPos, bool isClick) {
  if (m_lastEraserPos.isNull()) m_lastEraserPos = currentPos;
  QLineF movementLine(m_lastEraserPos, currentPos);
  QRectF detectionRect = QRectF(m_lastEraserPos, currentPos).normalized().adjusted(-5, -5, 5, 5);

  QGraphicsItem *genericItemUnderMouse = nullptr;
  QList<QGraphicsItem *> candidates = scene()->items(detectionRect);

  QPainterPathStroker stroker;
  stroker.setWidth(m_currentLineWidth + 15.0); // Un poco más de margen para velocidad
  stroker.setCapStyle(Qt::RoundCap);
  stroker.setJoinStyle(Qt::RoundJoin);
  stroker.setCurveThreshold(0.1);

  for (QGraphicsItem *item : candidates) {
    bool isHit = false;

    QPainterPath mouseTrajectory;
    mouseTrajectory.moveTo(item->mapFromScene(m_lastEraserPos));
    mouseTrajectory.lineTo(item->mapFromScene(currentPos));

    if (auto pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(item)) {
      QPainterPath strokePath = stroker.createStroke(pathItem->path());
      isHit = strokePath.intersects(mouseTrajectory) || strokePath.contains(item->mapFromScene(currentPos));
    }
    else if (auto lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(item)) {
      QPainterPath linePath;
      linePath.moveTo(lineItem->line().p1());
      linePath.lineTo(lineItem->line().p2());
      QPainterPath strokePath = stroker.createStroke(linePath);
      isHit = strokePath.intersects(mouseTrajectory) || strokePath.contains(item->mapFromScene(currentPos));
    }
    else if (auto pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(item)) {
      QRectF imageRect = pixmapItem->boundingRect();
      isHit = mouseTrajectory.intersects(imageRect) || imageRect.contains(item->mapFromScene(currentPos));
    }

    if (isHit) {
      genericItemUnderMouse = item;
      break;
    }
  }

  if (isClick) {
    if (genericItemUnderMouse) {
      if (genericItemUnderMouse == m_hoveredItem) m_hoveredItem = nullptr;
      scene()->removeItem(genericItemUnderMouse);
      delete genericItemUnderMouse;
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

  m_lastEraserPos = currentPos;
}

void VirtualCanvas::clearHoverEffect() {
  if (m_hoveredItem) {
    m_hoveredItem->setOpacity(1.0);
    m_hoveredItem = nullptr;
  }
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

void VirtualCanvas::pasteFromClipboard(QPointF targetPos) {
  const QClipboard *clipboard = QApplication::clipboard();
  const QMimeData *mimeData = clipboard->mimeData();

  if (mimeData->hasImage()) {
    QImage image = qvariant_cast<QImage>(mimeData->imageData());
    
    if (!image.isNull()) {
      QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(image));
      
      // QPointF viewCenter = mapToScene(viewport()->rect().center());
      // pixmapItem->setPos(viewCenter);
      // QRectF itemRect = pixmapItem->boundingRect();
      // pixmapItem->setPos(targetPos.x() - itemRect.width() / 2, targetPos.y() - itemRect.height() / 2);
      pixmapItem->setPos(targetPos);
      
      pixmapItem->setFlags(
        QGraphicsItem::ItemIsMovable | 
        QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemSendsGeometryChanges
      );
      
      // scene()->addItem(pixmapItem);
      if (m_undoStack) {
        m_undoStack->push(new AddCommand(pixmapItem, scene()));
      } else {
        // Fallback in case m_undoStack is not initialized yet
        scene()->addItem(pixmapItem);
      }
    }
  }
}

// void VirtualCanvas::mouseMoveEvent2(QMouseEvent *event) {
//   QPointF currentPoint = mapToScene(event->pos());

//   if (m_isEraserMode) {
//     QPainterPathStroker stroker;
//     stroker.setWidth(m_currentLineWidth + 10.0);
//     stroker.setCapStyle(Qt::RoundCap);
//     stroker.setJoinStyle(Qt::RoundJoin);

//     QGraphicsPathItem *itemUnderMouse = nullptr;

//     // Buscamos candidatos cerca del mouse
//     QList<QGraphicsItem *> candidates = scene()->items(currentPoint);
//     //qDebug() << "Clic detectado en:" << candidates;

//     for (QGraphicsItem *item : candidates) {
//       if (auto p = qgraphicsitem_cast<QGraphicsPathItem*>(item)) {
//         // Generamos el área de colisión real (el "anillo" alrededor de la línea)
//         QPainterPath strokePath = stroker.createStroke(p->path());

//         // Verificamos si el mouse está sobre ese anillo
//         if (strokePath.contains(p->mapFromScene(currentPoint))) {
//           itemUnderMouse = p;
//           break;
//         }
//       }
//     }

//     // --- LÓGICA A: BORRADO (Si hay clic presionado) ---
//     if (event->buttons() & Qt::LeftButton) {
//       if (itemUnderMouse) {
//         scene()->removeItem(itemUnderMouse);
//         if (itemUnderMouse == m_hoveredItem) m_hoveredItem = nullptr;
//         delete itemUnderMouse;
//       }
//       return;
//     }

//     // --- LÓGICA B: HOVER (Si no hay clic) ---
//     if (itemUnderMouse != m_hoveredItem) {
//       if (m_hoveredItem) m_hoveredItem->setOpacity(1.0);
//       if (itemUnderMouse) itemUnderMouse->setOpacity(0.5);
//       m_hoveredItem = itemUnderMouse;
//     }

//     event->accept();
//   }

//   // if (m_isEraserMode && (event->buttons() & Qt::LeftButton)) {
//   //     // 1. Creamos un área de búsqueda sensible (un cuadrado de 10x10 píxeles)
//   //     qreal margin = 5.0;
//   //     QRectF eraserArea(currentPoint.x() - margin, currentPoint.y() - margin, margin * 2, margin * 2);

//   //     // 2. Buscamos TODOS los items en esa área
//   //     QList<QGraphicsItem *> foundItems = this->scene()->items(eraserArea);

//   //     for (QGraphicsItem *item : foundItems) {
//   //         // 3. Verificamos si es un segmento de línea (Type 6) o un Path (Type 9)
//   //         if (item->type() == QGraphicsLineItem::Type || item->type() == QGraphicsPathItem::Type) {
//   //             this->scene()->removeItem(item);
//   //             delete item;
//   //         }
//   //     }
//   //     return;
//   // }

//   if (isDrawing && (event->buttons() & Qt::LeftButton) && m_currentPathItem) {
//     // Añadimos línea al camino existente
//     m_currentPath.lineTo(currentPoint);
//     // Actualizamos el dibujo del item
//     m_currentPathItem->setPath(m_currentPath);
//     event->accept();
//     return;

//     // addLineSegment(lastPoint, currentPoint);
//     // lastPoint = currentPoint;
//   }

//   QGraphicsView::mouseMoveEvent(event);
// }

// void VirtualCanvas::processEraserDetection(const QPointF &pos, bool isClick) {
//   qreal padding = 2.0;
//   QRectF detectionRect(pos.x() - padding, pos.y() - padding, padding * 2, padding * 2);

//   QGraphicsItem *genericItemUnderMouse = nullptr; 
//   QList<QGraphicsItem *> candidates = scene()->items(detectionRect);

//   QPainterPathStroker stroker;
//   stroker.setWidth(m_currentLineWidth + 10.0);
//   stroker.setCapStyle(Qt::RoundCap);
//   stroker.setJoinStyle(Qt::RoundJoin);
//   stroker.setCurveThreshold(0.1);

//   for (QGraphicsItem *item : candidates) {
//     QPainterPath shapePath;

//     if (auto pathItem = qgraphicsitem_cast<QGraphicsPathItem*>(item)) {
//       shapePath = pathItem->path();
//     }
//     else if (auto lineItem = qgraphicsitem_cast<QGraphicsLineItem*>(item)) {
//       QPainterPath linePath;
//       linePath.moveTo(lineItem->line().p1());
//       linePath.lineTo(lineItem->line().p2());
//       shapePath = linePath;
//     }

//     if (!shapePath.isEmpty()) {
//       QPainterPath strokePath = stroker.createStroke(shapePath);
//       if (strokePath.contains(item->mapFromScene(pos))) {
//         genericItemUnderMouse = item;
//         break;
//       }
//     }
//   }

//   if (isClick) {
//     if (genericItemUnderMouse) {
//       if (genericItemUnderMouse == m_hoveredItem) m_hoveredItem = nullptr;
//       scene()->removeItem(genericItemUnderMouse);
//       delete genericItemUnderMouse;
//     }
//   } else {
//     if (genericItemUnderMouse != m_hoveredItem) {
//       clearHoverEffect();
//       if (genericItemUnderMouse) {
//         genericItemUnderMouse->setOpacity(0.5);
//         m_hoveredItem = genericItemUnderMouse;
//       }
//     }
//   }
// }
