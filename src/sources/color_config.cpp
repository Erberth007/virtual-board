#include "../headers/color_config.h"

const QMap<QString, QColor> colorMap = {
  {"colorButtonBlack", Qt::black},
  {"colorButtonRed", Qt::red},
  {"colorButtonBlue", Qt::blue},
  {"colorButtonGreen", Qt::darkGreen},
  {"colorButtonYellow", Qt::yellow},
  {"colorButtonCyan", Qt::cyan},
  {"colorButtonSandy", QColor("#F7007D")},
  {"colorButtonOrange", QColor("orange")},
  {"colorButtonPurple", QColor("purple")},
  {"colorButtonBrown", QColor("brown")},
  {"colorButtonLime", QColor("lime")},
  {"colorButtonSilver", QColor("silver")},
  {"colorButtonTeal", QColor("#2A9D8F")}
};

const QStringList colorOrder = {
  "colorButtonBlack",
  "colorButtonRed",
  "colorButtonBlue",
  "colorButtonGreen",
  "colorButtonSandy",
  "colorButtonOrange",
  "colorButtonTeal",
  "colorButtonPurple"
};

const QString defaultColorButton = "colorButtonBlack";