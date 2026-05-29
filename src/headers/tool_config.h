#ifndef TOOL_CONFIG_H
#define TOOL_CONFIG_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QVector>

#include "canvas_mode.h"

struct ToolConfig
{
  CanvasMode mode;
  QString label;
  Qt::Key shortcutKey;
  QString iconPath;
};

struct WidthOption
{
  int width;
  QString objectName;
};

extern const QList<ToolConfig> toolConfigs;
extern const QVector<WidthOption> lineWidthOptions;
extern const int defaultLineWidthIndex;

#endif // TOOL_CONFIG_H