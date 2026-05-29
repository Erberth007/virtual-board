#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include <QWidget>
#include <QString>

class UiHelpers {
  public:
    static QWidget* wrapInFloatingFrame(
      QWidget* content,
      const QString& objectName,
      QWidget* parent = nullptr
    );
};

#endif // UI_HELPERS_H