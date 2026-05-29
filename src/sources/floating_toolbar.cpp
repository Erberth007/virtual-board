#include "../headers/floating_toolbar.h"

#include <QVBoxLayout>
#include <QToolButton>
#include <QButtonGroup>
#include <QFrame>
#include <QIcon>
#include <QSize>
#include <QAbstractButton>
#include <QVariant>

#include "../headers/tool_config.h"
#include "../headers/tool_button_custom.h"
#include "../headers/separator.h"

FloatingToolbar::FloatingToolbar(QWidget* parent) : QFrame(parent), m_toolGroup(new QButtonGroup(this)) {
  setupUi();
  setupConnections();
  setupStyles();

  adjustSize();
  raise();
}

void FloatingToolbar::setupUi() {
  setObjectName("toolbarContainer");

  auto* toolLayout = new QVBoxLayout(this);
  toolLayout->setContentsMargins(8, 4, 8, 4);
  toolLayout->setSpacing(10);

  m_toolGroup->setExclusive(true);

  for (const auto& config : toolConfigs) {
    auto* button = new ToolButtonCustom(this);

    button->setIcon(QIcon(QString(":/src/icons/%1").arg(config.iconPath)));
    button->setIconSize(QSize(24, 24));

    int shortcutNum = (config.shortcutKey - Qt::Key_0) % 10;
    button->setText(QString::number(shortcutNum));

    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setCheckable(true);
    button->setFixedSize(40, 40);
    button->setCursor(Qt::PointingHandCursor);

    button->setProperty("mode", static_cast<int>(config.mode));
    button->setToolTip(QString("Herramienta %1 (%2)").arg(config.label).arg((config.shortcutKey - Qt::Key_0) % 10));

    m_toolGroup->addButton(button);
    toolLayout->addWidget(button);

    if (config.mode == CanvasMode::Pen) {
      button->setChecked(true);
    }
  }

  auto *separator = new Separator(this, Separator::Horizontal);
  toolLayout->addWidget(separator);

  addExtraActionButton();
}

void FloatingToolbar::setupConnections() {
  connect(
    m_toolGroup,
    QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
    this,
    [this](QAbstractButton *button) {
      int modeValue = button->property("mode").toInt();
      auto selectedMode = static_cast<CanvasMode>(modeValue);
      emit toolSelected(selectedMode);
    }
  );
}

void FloatingToolbar::setupStyles() {
  setStyleSheet(
    "#toolbarContainer {"
      "background-color: rgba(255, 255, 255, 0.86);"
      "border: 1px solid #e0e0e0;"
      "border-radius: 8px;"
      "padding: 4px;"
    "}"

    "QToolButton {"
      "border: none;"
      "border-radius: 6px;"
      "background-color: transparent;"
    "}"

    "QToolButton:hover {"
      "background-color: #f5f5f5;"
    "}"

    "QToolButton:checked {"
      "background-color: #eaddff;"
    "}"
  );
}

void FloatingToolbar::triggerShortcut(int key) {
  for (const auto& config : toolConfigs) {
    if (key == config.shortcutKey) {
      for (QAbstractButton* btn : m_toolGroup->buttons()) {
        if (btn->property("mode").toInt() == static_cast<int>(config.mode)) {
          btn->click();
          return;
        }
      }
    }
  }
}

void FloatingToolbar::addExtraActionButton() {
  auto* btnExtra = new QToolButton(this);

  btnExtra->setIcon(QIcon(":/src/icons/011_clear_tool.svg"));

  btnExtra->setIconSize(QSize(24, 24));
  btnExtra->setFixedSize(40, 40);
  btnExtra->setCursor(Qt::PointingHandCursor);
  btnExtra->setToolTip("Limpiar");
  btnExtra->setCheckable(false);

  btnExtra->setStyleSheet(
    "QToolButton {"
      "border: none;"
      "border-radius: 8px;"
      "background: transparent;"
    "}"
    "QToolButton:hover {"
      "background-color: #f0f0f0;"
    "}"
  );

  connect(
    btnExtra,
    &QToolButton::clicked,
    this,
    &FloatingToolbar::clearRequested
  );

  layout()->addWidget(btnExtra);
}
