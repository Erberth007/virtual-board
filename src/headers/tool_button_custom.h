#ifndef TOOL_BUTTON_CUSTOM_H
#define TOOL_BUTTON_CUSTOM_H

#include <QToolButton>

class QPaintEvent;

class ToolButtonCustom : public QToolButton {
  public:
    explicit ToolButtonCustom(QWidget *parent = nullptr);

  protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // TOOL_BUTTON_CUSTOM_H
