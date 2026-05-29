#include "../headers/tool_config.h"

#include <QVector>

const QList<ToolConfig> toolConfigs = {
  {
    CanvasMode::Hand,
    "Mano",
    Qt::Key_1,
    "001_hand_tool.svg"
  },
  {
    CanvasMode::Selection,
    "Puntero",
    Qt::Key_2,
    "002_selection_tool.svg"
  },
  {
    CanvasMode::Rectangle,
    "Cuadrado",
    Qt::Key_3,
    "003_rectangle_tool.svg"
  },
  {
    CanvasMode::Ellipse,
    "Círculo",
    Qt::Key_4,
    "004_ellipse_tool.svg"
  },
  {
    CanvasMode::Arrow,
    "Flecha",
    Qt::Key_5,
    "005_arrow_tool.svg"
  },
  {
    CanvasMode::Line,
    "Línea",
    Qt::Key_6,
    "006_line_tool.svg"
  },
  {
    CanvasMode::Pen,
    "Lápiz",
    Qt::Key_7,
    "007_pen_tool.svg"
  },
  {
    CanvasMode::Text,
    "Texto",
    Qt::Key_8,
    "008_text_tool.svg"
  },
  {
    CanvasMode::Image,
    "Imagen",
    Qt::Key_9,
    "009_imagen_tool.svg"
  },
  {
    CanvasMode::Eraser,
    "Borrador",
    Qt::Key_0,
    "010_eraser_tool.svg"
  }
};

const QVector<WidthOption> lineWidthOptions = {
  {4, "widthThin"},
  {6, "widthMedium"},
  {8, "widthThick"}
};

const int defaultLineWidthIndex = 1;