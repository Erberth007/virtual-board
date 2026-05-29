#ifndef VIRTUAL_CANVAS_H
#define VIRTUAL_CANVAS_H

#include <QGraphicsView>
#include <QSet>

#include "canvas_mode.h"
#include "tool_config.h"
#include "color_config.h"

class Page;
class QColor;
class QMouseEvent;

class VirtualCanvas : public QGraphicsView {
  Q_OBJECT

  public:
    explicit VirtualCanvas(QWidget *parent = nullptr);
    ~VirtualCanvas() = default;

    void setCurrentPage(Page *page);
    void setMode(CanvasMode mode);
    void clearCanvas();
    void setCurrentColor(const QColor &color);
    void setCurrentLineWidth(int width);

    void pasteFromClipboard(QPointF targetPos);

  protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

  private:
    void setupUi();
    Page *m_currentPage = nullptr;
    CanvasMode m_currentMode = CanvasMode::Pen;

    void updateCursor();

    QColor m_currentColor = colorMap[defaultColorButton];
    int m_currentLineWidth = lineWidthOptions[defaultLineWidthIndex].width;

    QPainterPath m_currentPath;
    QGraphicsPathItem *m_currentPathItem = nullptr;

    QPointF m_lastEraserPos;
    QGraphicsItem *m_hoveredItem = nullptr;
    qreal m_eraserSize = 15;
    void processEraserDetection(const QPointF &scenePos, bool isLeftButtonPressed);
    void clearHoverEffect();

    QPointF m_startPoint;
    void createDefaultPathItem();
    QPointF calculateSnappedPoint(const QPointF &start, const QPointF &end);

    void recalculateCurrentShape(QPointF scenePos, bool isShiftPressed);

    QSet<QGraphicsItem*> m_erasedItems;
    QPainterPathStroker m_eraserStroker;
    qreal m_nextZValue = 0;

    void updateItemsInteractionFlags(bool enabled);

    bool m_isTemporaryCursorActive = false;
    // QCursor m_temporaryCursor;
};

#endif // VIRTUAL_CANVAS_H
