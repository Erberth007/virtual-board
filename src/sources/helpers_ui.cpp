#include "../headers/helpers_ui.h"

#include <QFrame>
#include <QHBoxLayout>

QWidget* UiHelpers::wrapInFloatingFrame(QWidget* content, const QString& objectName, QWidget* parent) {
  QFrame* container = new QFrame(parent);
  container->setObjectName(objectName);

  auto* layout = new QHBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(content);

  container->setStyleSheet(
    QString(
      "#%1 {"
        "background: qlineargradient("
        "x1:0, y1:0, x2:0, y2:1,"
        "stop:0 #E5E5E5, stop:1 #E0E0E0);"
      "}"
    ).arg(objectName)
  );

  container->adjustSize();
  container->raise();

  return container;
}