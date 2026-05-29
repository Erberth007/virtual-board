#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <QGraphicsView>

class Separator : public QFrame {
  Q_OBJECT

  public:
    enum Orientation {
      Vertical,
      Horizontal
    };
    explicit Separator(QWidget *parent = nullptr, Orientation orientation = Vertical, int thickness = 2);
    ~Separator() = default;

    void setBaseColor(const QColor &color);

    private:
      void setupUI();
      void applyStyle();

    private:
      Orientation m_orientation;
      int m_thickness;

      QColor m_baseColor = QColor(5, 87, 230, 80);
};

#endif // SEPARATOR_H