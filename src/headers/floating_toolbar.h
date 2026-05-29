#ifndef FLOATING_TOOLBAR_H
#define FLOATING_TOOLBAR_H

#include <QFrame>
#include <QButtonGroup>

#include "canvas_mode.h"

class QAbstractButton;

class FloatingToolbar : public QFrame {
  Q_OBJECT

  public:
    explicit FloatingToolbar(QWidget *parent = nullptr);
    ~FloatingToolbar() = default;
    void triggerShortcut(int key);

  signals:
    void toolSelected(CanvasMode mode);
    void clearRequested();

  private:
    QButtonGroup *m_toolGroup;

    void setupUi();
    void setupConnections();
    void setupStyles();
    void addExtraActionButton();
};

#endif // FLOATING_TOOLBAR_H