#include "../headers/main_window.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QVBoxLayout>
#include <QScreen>
#include <QPushButton>
#include <QAction>
#include <QDebug>

#include "../headers/secondary_view.h"
#include "../headers/tool_button_custom.h"
#include "../headers/tool_config.h"
#include "../headers/color_config.h"
#include "../headers/floating_toolbar.h"
#include "../headers/helpers_ui.h"
#include "../headers/floating_statusbar.h"
#include "../headers/virtual_canvas.h"

// ***************** public *****************
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow) {
  ui->setupUi(this);
  initPageSystem();
  generateVirtualCanvas();
  openSecondaryDisplay();
  configActions();
  generateToolbar();
  generateStatusbar();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);

  if (m_toolbar) {
    int y = (this->height() - m_toolbar->height()) / 2;
    m_toolbar->move(0, y);
    m_toolbar->show();
  }
  if(m_statusbar) {
    m_statusbar->move(0, this->height() - m_statusbar->height());
    m_statusbar->setMinimumWidth(this->width());
  }
}

// ***************** protected *****************
void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->isAutoRepeat()) return;

  int key = event->key();

  bool controlPressed = (event->modifiers() & Qt::ControlModifier);
  bool shiftPressed = (event->modifiers() & Qt::ShiftModifier);
  bool keypadPressed = (event->modifiers() & Qt::KeypadModifier);

  if (controlPressed && !m_pages_array.isEmpty() && m_currentPageIndex >= 0) {
    QUndoStack *currentStack = m_pages_array.at(m_currentPageIndex)->undoStack();
    
    if (currentStack) {
      // Ctrl + Z
      if (key == Qt::Key_Z && !shiftPressed) {
        // qDebug() << currentStack->index();
        currentStack->undo();
        qDebug() << "Ctrl + Z ...";
        event->accept();
        return;
      }
      // Ctrl + Shift + Z || Ctrl + Y
      if ((key == Qt::Key_Z && shiftPressed) || key == Qt::Key_Y) {
        // qDebug() << currentStack->index();
        currentStack->redo();
        qDebug() << "Ctrl + Y ...";
        event->accept();
        return;
      }
    }
  }

  if (key == Qt::Key_C) { m_cKeyPressed = true; return; }
  if (key == Qt::Key_S) { m_sKeyPressed = true; return; }
  if (key == Qt::Key_P) { m_pKeyPressed = true; return; }

  if(keypadPressed) {
    if (key > Qt::Key_0 && key < Qt::Key_9) {
      int index = key - Qt::Key_0;
      QString targetGroup = "colorGroup";
      if (controlPressed) targetGroup = "lineWidthGroup";
      if (!targetGroup.isEmpty()) {
        QFrame *group = this->findChild<QFrame*>(targetGroup);
        if (group) {
          auto buttons = group->findChildren<QPushButton*>();
          if (index > 0 && (index - 1) < buttons.size()){
            buttons[index - 1]->click();
          }
        }
        event->accept();
        return;
      }
    }
  }
  // if(keypadPressed && (m_cKeyPressed || m_sKeyPressed)) {
  //   if (key > Qt::Key_0 && key < Qt::Key_9) {
  //     int index = key - Qt::Key_0;
  //     QString targetGroup = "";
  //     if (m_cKeyPressed) targetGroup = "colorGroup";
  //     else if (m_sKeyPressed) targetGroup = "lineWidthGroup";
  //     //else if (m_pKeyPressed) targetGroup = "pointSizeGroup";
  //     if (!targetGroup.isEmpty()) {
  //       QFrame *group = this->findChild<QFrame*>(targetGroup);
  //       if (group) {
  //         auto buttons = group->findChildren<QPushButton*>();
  //         if (index > 0 && (index - 1) < buttons.size()){
  //           buttons[index - 1]->click();
  //         }
  //       }
  //       event->accept();
  //       return;
  //     }
  //   }
  // }
  else {
    if(!keypadPressed) {
      m_toolbar->triggerShortcut(event->key());
    }
  }

  QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  if (event->isAutoRepeat()) return;

  int key = event->key();
  if (key == Qt::Key_C) m_cKeyPressed = false;
  if (key == Qt::Key_S) m_sKeyPressed = false;

  QMainWindow::keyReleaseEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (m_secondaryView) {
      m_secondaryView->close();
      delete m_secondaryView;
      m_secondaryView = nullptr;
  }
  QMainWindow::closeEvent(event);
}

// ***************** private *****************
void MainWindow::generateVirtualCanvas() {
  m_canvasView = new VirtualCanvas(this);
  
  if (!m_pages_array.isEmpty() && m_currentPageIndex == 0) {
    m_canvasView->setCurrentPage(m_pages_array.at(m_currentPageIndex));
  }

  auto* layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
  if (!layout) {
    layout = new QVBoxLayout(ui->centralwidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    ui->centralwidget->setLayout(layout);
  }
  
  while (QLayoutItem* item = layout->takeAt(0)) {
    if (QWidget* widget = item->widget()) { widget->deleteLater(); }
    delete item;
  }

  layout->addWidget(m_canvasView, 1);
  ui->centralwidget->setContentsMargins(0, 0, 0, 0);
}

void MainWindow::openSecondaryDisplay() {
  QList<QScreen *> screens = QGuiApplication::screens();
  
  if (screens.size() > 1 && m_canvasView) {
    if (!m_secondaryView) {
      m_secondaryView = new SecondaryView(m_canvasView->scene());
      m_secondaryView->setAttribute(Qt::WA_QuitOnClose, false);
    } else {
      m_secondaryView->setScene(m_canvasView->scene());
    }
    QRect secondaryRect = screens.at(1)->geometry();
    m_secondaryView->setGeometry(secondaryRect);
    m_secondaryView->showFullScreen();
  }
}

// void MainWindow::openSecondaryDisplay() {
//   QList<QScreen *> screens = QGuiApplication::screens();
  
//   if (screens.size() > 1 && m_canvasView) {
//     SecondaryView *mirrorView = new SecondaryView(m_canvasView->scene());
//     mirrorView->setAttribute(Qt::WA_QuitOnClose, false);
//     QRect secondaryRect = screens.at(1)->geometry();
//     mirrorView->setGeometry(secondaryRect);
//     mirrorView->showFullScreen();
//   }
// }

void MainWindow::configActions() {
  QAction *closeAction = new QAction(this);
  closeAction->setShortcut(QKeySequence("Ctrl+Shift+Q"));
  connect(closeAction, &QAction::triggered, this, &MainWindow::close);
  this->addAction(closeAction);
}

void MainWindow::generateToolbar() {
  m_toolbar = new FloatingToolbar(this);
  if (m_canvasView) {
    connect(
      m_toolbar,
      &FloatingToolbar::toolSelected,
      m_canvasView,
      &VirtualCanvas::setMode
    );
    connect(
      m_toolbar,
      &FloatingToolbar::clearRequested,
      m_canvasView,
      &VirtualCanvas::clearCanvas
    );
  }
}

void MainWindow::generateStatusbar() {
  m_statusbar = new FloatingStatusbar(this);
  connect(
    m_statusbar,
    &FloatingStatusbar::toggleToolbarRequested,
    this,
    [this]() {
      m_toolbar->setVisible(!m_toolbar->isVisible());
    }
  );
  connect(
    m_statusbar,
    &FloatingStatusbar::exitRequested,
    this,
    &MainWindow::close
  );

  if (m_canvasView) {
    connect(
      m_statusbar,
      &FloatingStatusbar::colorChanged,
      this,
      [this](QAbstractButton *button) {
        QString btnName = button->objectName();
        if (colorMap.contains(btnName)) {
            m_canvasView->setCurrentColor(colorMap[btnName]);
        }
      }
    );
    connect(
      m_statusbar,
      &FloatingStatusbar::lineWidthChanged,
      this,
      [this](QAbstractButton *button) {
        QString btnName = button->objectName();
        for (const WidthOption &option : lineWidthOptions) {
          if (option.objectName == btnName) {
            m_canvasView->setCurrentLineWidth(option.width);
            break;
          }
        }
        // if (btnName == "widthThin") m_canvasView->setCurrentLineWidth(4);
        // else if (btnName == "widthMedium") m_canvasView->setCurrentLineWidth(6);
        // else if (btnName == "widthThick") m_canvasView->setCurrentLineWidth(8);
      }
    );
    
    
    connect(m_statusbar, &FloatingStatusbar::addPageRequested, this, &MainWindow::addPage);
    connect(m_statusbar, &FloatingStatusbar::prevPageRequested, this, &MainWindow::prevPage);
    connect(m_statusbar, &FloatingStatusbar::nextPageRequested, this, &MainWindow::nextPage);
    updatePageDisplay();
  }
}

void MainWindow::initPageSystem() {
  Page *firstPage = new Page(this);
  m_pages_array.append(firstPage);
  m_currentPageIndex = 0;
  if (m_statusbar) {
    m_statusbar->updateNavigationButtons(false, false);
  }
}

void MainWindow::addPage() {
  Page *newPage = new Page(this);
  m_pages_array.append(newPage);
  m_currentPageIndex = m_pages_array.size() - 1;
  updatePageDisplay();
}

void MainWindow::nextPage() {
  if (m_currentPageIndex < m_pages_array.size() - 1) {
    m_currentPageIndex++;
    updatePageDisplay();
  }
}

void MainWindow::prevPage() {
  if (m_currentPageIndex > 0) {
    m_currentPageIndex--;
    updatePageDisplay();
  }
}

void MainWindow::updatePageDisplay() {
  if (m_currentPageIndex >= 0 && m_currentPageIndex < m_pages_array.size()) {
    Page* activePage = m_pages_array.at(m_currentPageIndex);
    
    m_canvasView->setCurrentPage(activePage);
    if (m_secondaryView && activePage && activePage->scene()) {
      m_secondaryView->setScene(activePage->scene());
    }
    
    bool enablePrev = (m_currentPageIndex > 0);
    bool enableNext = (m_currentPageIndex < m_pages_array.size() - 1);

    if (m_statusbar) {
      m_statusbar->updateNavigationButtons(enablePrev, enableNext);
      m_statusbar->setPageLabelText(m_currentPageIndex + 1, m_pages_array.size());
    }

    if (m_canvasView) {
      m_canvasView->setFocus();
    }
  }
}


// void MainWindow::generateVirtualCanvas() {
//   m_canvasView = new VirtualCanvas(this);
//   auto* layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());

//   if (!layout) {
//     layout = new QVBoxLayout(ui->centralwidget);
//     layout->setContentsMargins(0, 0, 0, 0);
//     layout->setSpacing(0);
//     ui->centralwidget->setLayout(layout);
//   }

//   while (QLayoutItem* item = layout->takeAt(0)) {
//     if (QWidget* widget = item->widget()) {
//       widget->deleteLater();
//     }
//     delete item;
//   }

//   layout->addWidget(m_canvasView, 1);
//   ui->centralwidget->setContentsMargins(0, 0, 0, 0);
// }