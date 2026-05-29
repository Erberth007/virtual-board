#include "../headers/tool_button_custom.h"

#include <QPainter>
#include <QStyleOptionToolButton>

ToolButtonCustom::ToolButtonCustom(QWidget *parent) : QToolButton(parent) {}

void ToolButtonCustom::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QStyleOptionToolButton opt;
  initStyleOption(&opt);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

  QRect qRect = rect();
  QPixmap pix = icon().pixmap(iconSize());
  QRect iconRect = QRect(0, 0, pix.width(), pix.height());
  iconRect.moveCenter(qRect.center());
  iconRect.translate(-2, -2);
  painter.drawPixmap(iconRect, pix);

  if (!text().isEmpty()) {
    painter.setPen(isChecked() ? QColor("#0557e6") : QColor("#888888"));
    QFont f = font();
    f.setPointSize(isChecked() ? 10 : 8);
    f.setBold(isChecked() ? true : false);
    painter.setFont(f);
    QRect textRect = qRect.adjusted(0, 0, -5, -3);
    painter.drawText(textRect, Qt::AlignRight | Qt::AlignBottom, text());
  }
}
