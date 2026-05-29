#ifndef BUTTON_FACTORY_H
#define BUTTON_FACTORY_H

#include <QPushButton>
#include <QString>
#include <QSize>

class ButtonFactory {
  public:
    static QPushButton *createIconButton(
      const QString &objectName,
      const QString &iconName,
      QWidget *parent = nullptr,
      const QSize &iconSize = QSize(22, 22),
      bool checkable = false
    );
};

#endif
