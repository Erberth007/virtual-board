#include "../headers/separator.h"
#include <QEvent>

Separator::Separator(QWidget *parent, Orientation orientation, int thickness) : QFrame(parent), m_orientation(orientation), m_thickness(thickness) {
  setupUI();
  applyStyle();
}

void Separator::setBaseColor(const QColor &color) {
  m_baseColor = color;
  applyStyle();
}

void Separator::setupUI() {
  if (m_orientation == Vertical)
    setFixedWidth(m_thickness);
  else
    setFixedHeight(m_thickness);
}

void Separator::applyStyle() {
  setStyleSheet(QString(R"(
    QFrame {
      background-color: rgba(%1, %2, %3, %4);
      border-radius: 2px;
      margin: 6px 4px;
    }
  )")
  .arg(m_baseColor.red())
  .arg(m_baseColor.green())
  .arg(m_baseColor.blue())
  .arg(m_baseColor.alpha()));
}
