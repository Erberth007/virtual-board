#ifndef VIRTUAL_CANVAS_H
#define VIRTUAL_CANVAS_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsPathItem>
#include <QGraphicsItem>
#include "addCommand.h"
#include <QUndoStack>

#include "canvas_mode.h"


#include "page.h"

class VirtualCanvas : public QGraphicsView {
  Q_OBJECT

  public:
    explicit VirtualCanvas(QWidget *parent = nullptr);
    ~VirtualCanvas() = default;
    void setCurrentPage(Page *page);



    void setMode(CanvasMode mode);

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;
    void setCurrentLineWidth(int width);
    int currentLineWidth() const;

    void clearCanvas();

    void deleteSelectedItems();

  protected:
    void resizeEvent(QResizeEvent *event);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void keyPressEvent(QKeyEvent *event);

  private:
    QGraphicsScene *drawingScene;
    void setupScene();
    void setupGridTexture();

    CanvasMode m_currentMode = CanvasMode::Pen;
    void updateCursor();

    int m_currentLineWidth = 8;
    QColor m_currentColor = Qt::black;

    QPainterPath m_currentPath; 
    QGraphicsPathItem *m_currentPathItem = nullptr;

    QGraphicsItem *m_hoveredItem = nullptr;
    void processEraserDetection(const QPointF &pos, bool isClick);
    void clearHoverEffect();

    QGraphicsLineItem* m_currentLineItem = nullptr;
    QPointF m_startPoint;
    QPointF calculateSnappedPoint(const QPointF &start, const QPointF &end);

    QPointF m_lastEraserPos;

    void pasteFromClipboard(QPointF targetPos);

    QGraphicsRectItem* m_currentRectItem = nullptr;
    QGraphicsEllipseItem* m_currentEllipseItem = nullptr;
    QGraphicsLineItem* m_currentArrowLineItem = nullptr;
    QGraphicsPolygonItem* m_currentArrowHeadItem = nullptr;

    QUndoStack *m_undoStack = nullptr;
};

// VirtualCanvas::VirtualCanvas(/* args */)
// {
// }

// VirtualCanvas::~VirtualCanvas()
// {
// }

#endif // VIRTUAL_CANVAS_H
