#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QVector>

#include "page.h"

class QWidget;
class QResizeEvent;
class QKeyEvent;

class VirtualCanvas;
class FloatingToolbar;
class FloatingStatusbar;
class SecondaryView;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resizeEvent(QResizeEvent *event);

  protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

  private:
    Ui::MainWindow *ui;

    VirtualCanvas *m_canvasView = nullptr;
    SecondaryView *m_secondaryView = nullptr;
    void generateVirtualCanvas();
    void openSecondaryDisplay();
    void configActions();

    FloatingToolbar *m_toolbar = nullptr;
    void generateToolbar();
    FloatingStatusbar *m_statusbar = nullptr;
    void generateStatusbar();

    QVector<Page*> m_pages_array;
    int m_currentPageIndex = -1;
    void initPageSystem();

    void addPage();
    void nextPage();
    void prevPage();
    void updatePageDisplay();

    bool m_cKeyPressed = false;
    bool m_sKeyPressed = false;
    bool m_pKeyPressed = false;
};

#endif // MAIN_WINDOW_H
