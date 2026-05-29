#include "../headers/button_factory.h"

#include <QIcon>
#include <QSize>

QPushButton *ButtonFactory::createIconButton(
  const QString &objectName,
  const QString &iconName,
  QWidget *parent,
  const QSize &iconSize,
  bool checkable
) {
  QPushButton *btn = new QPushButton(parent);
  btn->setObjectName(objectName);
  btn->setCursor(Qt::PointingHandCursor);

  btn->setIcon(QIcon(QString(":/src/icons/%1").arg(iconName)));
  btn->setIconSize(iconSize);
  btn->setCheckable(checkable);

  int padding = 8;
  int buttonWidth = iconSize.width() + padding;
  int buttonHeight = iconSize.height() + padding;
  btn->setFixedSize(buttonWidth, buttonHeight);

  btn->setStyleSheet(QString(
    "#%1 {"
      "background: qlineargradient("
      "x1:0, y1:0, x2:0, y2:1,"
      "stop:0 #E5E5E5, stop:1 #E0E0E0);"
      "border: 1px solid #00000000;"
    "}"
  ).arg(objectName));

  return btn;
}
