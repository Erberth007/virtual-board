#include "../headers/floating_statusbar.h"

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QPainter>
#include <QLabel>

#include "../headers/button_factory.h"
#include "../headers/tool_config.h"
#include "../headers/color_config.h"
#include "../headers/separator.h"

FloatingStatusbar::FloatingStatusbar(QWidget *parent) : QFrame(parent) {
  setupUi();
  setupStyles();

  adjustSize();
  raise();
}

void FloatingStatusbar::updateNavigationButtons(bool enablePrev, bool enableNext) {
  if (m_btnPrev) m_btnPrev->setEnabled(enablePrev);
  if (m_btnNext) m_btnNext->setEnabled(enableNext);
}

void FloatingStatusbar::setPageLabelText(int current, int total) {
  if (m_pageLabel) {
    m_pageLabel->setText(QString("Pág. %1/%2").arg(current).arg(total));
  }
}

void FloatingStatusbar::setupUi() {
  setObjectName("statusbarContainer");
  setFixedHeight(59);

  auto *m_layout = new QHBoxLayout(this);
  m_layout->setContentsMargins(16, 0, 16, 0);
  m_layout->setSpacing(18);

  QPushButton *toggleToolbarBtn = ButtonFactory::createIconButton("toggleToolbarBtn", "menu-icon.svg", this);
  connect(toggleToolbarBtn, &QPushButton::clicked, this, &FloatingStatusbar::toggleToolbarRequested);
  m_layout->addWidget(toggleToolbarBtn);

  m_layout->addWidget(createColorPaletteGroup());
  m_layout->addWidget(createLineWidthGroup());

  auto *separator = new Separator(this, Separator::Vertical);
  m_layout->addWidget(separator);
  
  m_layout->addWidget(createPagesGroup());
  m_layout->addStretch();
  
  QPushButton *exitBtn = ButtonFactory::createIconButton("exitBtn", "quit-icon.svg", this);
  connect(exitBtn, &QPushButton::clicked, this, &FloatingStatusbar::exitRequested);
  m_layout->addWidget(exitBtn);
}

void FloatingStatusbar::setupStyles() {
  setStyleSheet(R"(
    #statusbarContainer {
      background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E5E5E5, stop:1 #E0E0E0);
    }
  )");
}

QWidget *FloatingStatusbar::createColorPaletteGroup() {
  QFrame *group = new QFrame(this);
  group->setObjectName("colorGroup");
  group->setStyleSheet(
    "#colorGroup {"
      "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E5E5E5, stop:1 #E0E0E0);"
      "border: 1px solid #000;"
      "border-radius: 3px;"
      "min-height: 27px; max-height: 27px;"
    "}"
    "QPushButton {"
      "background: transparent; border: none;"
      "min-width: 29px; min-height: 27px; max-width: 29px; max-height: 27px;"
    "}"
    "QPushButton:checked {"
      "background-color: #D0D0D0;"
      "border: 1px solid #555; border-top: none; border-bottom: none;"
    "}"
  );

  QHBoxLayout *layout = new QHBoxLayout(group);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  QButtonGroup *buttonGroup = new QButtonGroup(this);
  buttonGroup->setExclusive(true);

  connect(
    buttonGroup,
    QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
    this,
    &FloatingStatusbar::colorChanged
  );

  for (const QString &name : colorOrder) {
    QPushButton *btn = new QPushButton(group);
    btn->setCheckable(true);
    btn->setObjectName(name);
    btn->setCursor(Qt::PointingHandCursor);

    QPixmap pix(18, 18);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(colorMap[name]);
    p.drawRect(2, 2, 14, 14);
    p.end();
    btn->setIcon(QIcon(pix));

    if (name == defaultColorButton) btn->setChecked(true);
    buttonGroup->addButton(btn);
    layout->addWidget(btn);
  }
  return group;
}

QWidget *FloatingStatusbar::createLineWidthGroup() {
  QFrame *group = new QFrame(this);
  group->setObjectName("lineWidthGroup");
  group->setStyleSheet(
    "#lineWidthGroup {"
      "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E5E5E5, stop:1 #E0E0E0);"
      "border: 1px solid #000;"
      "border-radius: 3px;"
      "min-height: 27px; max-height: 27px;"
    "}"
    "QPushButton {"
      "background: transparent; border: none;"
      "min-width: 29px; min-height: 27px; max-width: 29px; max-height: 27px;"
    "}"
    "QPushButton:checked {"
      "background-color: #D0D0D0;"
      "border: 1px solid #555; border-top: none; border-bottom: none;"
    "}"
  );

  QHBoxLayout *layout = new QHBoxLayout(group);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  QButtonGroup *buttonGroup = new QButtonGroup(this);
  buttonGroup->setExclusive(true);

  connect(
    buttonGroup,
    QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
    this,
    &FloatingStatusbar::lineWidthChanged
  );

  for (int i = 0; i < lineWidthOptions.size(); ++i) {
    QPushButton *btn = new QPushButton(group);
    btn->setCheckable(true);
    btn->setObjectName(lineWidthOptions[i].objectName);
    btn->setCursor(Qt::PointingHandCursor);

    QPixmap pix(24, 24);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, lineWidthOptions[i].width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawLine(4, 12, 20, 12);
    p.end();
    btn->setIcon(QIcon(pix));

    if (i == defaultLineWidthIndex) btn->setChecked(true); // Por defecto Medium
    buttonGroup->addButton(btn);
    layout->addWidget(btn);
  }
  return group;
}

QWidget *FloatingStatusbar::createPagesGroup() {
  QFrame *pagesGroup = new QFrame(this);
  pagesGroup->setObjectName("pagesGroup");
  pagesGroup->setStyleSheet(R"(
    #pagesGroup {
      background: transparent;
    }
  )");

  QHBoxLayout *pagesLayout = new QHBoxLayout(pagesGroup);
  pagesLayout->setContentsMargins(4, 2, 4, 2);
  pagesLayout->setSpacing(8);

  QPushButton *btnAdd = ButtonFactory::createIconButton("btnPageStatus", "new_page.svg", pagesGroup, QSize(30, 30));
  connect(btnAdd, &QPushButton::clicked, this, &FloatingStatusbar::addPageRequested);

  m_btnPrev = ButtonFactory::createIconButton("btnPagePrev", "prev_page.svg", pagesGroup, QSize(30, 30));
  connect(m_btnPrev, &QPushButton::clicked, this, &FloatingStatusbar::prevPageRequested);


  m_pageLabel = new QLabel("Pág. 1/1", pagesGroup);
  m_pageLabel->setStyleSheet(
    "font-weight: bold; "
    "color: #333333; "
    "padding-left: 4px; "
    "padding-right: 4px;"
  );
  m_pageLabel->setAlignment(Qt::AlignCenter);

  m_btnNext = ButtonFactory::createIconButton("btnPageNext", "next_page.svg", pagesGroup, QSize(30, 30));
  connect(m_btnNext, &QPushButton::clicked, this, &FloatingStatusbar::nextPageRequested);

  pagesLayout->addWidget(btnAdd);
  pagesLayout->addWidget(m_btnPrev);
  pagesLayout->addWidget(m_btnNext);
  pagesLayout->addWidget(m_pageLabel);
  
  return pagesGroup;
}
