#ifndef FLOATING_STATUSBAR_H
#define FLOATING_STATUSBAR_H

#include <QFrame>

class QAbstractButton;
class QPushButton;
class QLabel;

class FloatingStatusbar : public QFrame {
  Q_OBJECT

  public:
    explicit FloatingStatusbar(QWidget *parent = nullptr);
    ~FloatingStatusbar() = default;
    void updateNavigationButtons(bool enablePrev, bool enableNext);
    void setPageLabelText(int current, int total);

  signals:
    void toggleToolbarRequested();
    void exitRequested();
    void colorChanged(QAbstractButton *button);
    void lineWidthChanged(QAbstractButton *button);

    void addPageRequested();
    void prevPageRequested();
    void nextPageRequested();

  private:
    void setupUi();
    void setupStyles();

    QWidget *createColorPaletteGroup();
    QWidget *createLineWidthGroup();
    QWidget *createPagesGroup();

    QPushButton *m_btnPrev = nullptr;
    QPushButton *m_btnNext = nullptr;
    QLabel *m_pageLabel = nullptr;
};

#endif // FLOATING_STATUSBAR_H