QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/sources/secondary_view.cpp \
    src/sources/main.cpp \
    src/sources/main_window.cpp \
    src/sources/tool_button_custom.cpp \
    src/sources/tool_config.cpp \
    src/sources/color_config.cpp \
    src/sources/floating_toolbar.cpp \
    src/sources/helpers_ui.cpp \
    src/sources/button_factory.cpp \
    src/sources/floating_statusbar.cpp \
    src/sources/page.cpp \
    src/sources/virtual_canvas.cpp \
    src/sources/separator.cpp \
    src/sources/add_command.cpp \
    src/sources/remove_command.cpp \
    src/sources/custom_path_item.cpp \

HEADERS += \
    src/headers/secondary_view.h \
    src/headers/main_window.h \
    src/headers/tool_button_custom.h \
    src/headers/tool_config.h \
    src/headers/canvas_mode.h \
    src/headers/color_config.h \
    src/headers/floating_toolbar.h \
    src/headers/helpers_ui.h \
    src/headers/button_factory.h \
    src/headers/floating_statusbar.h \
    src/headers/page.h \
    src/headers/virtual_canvas.h \
    src/headers/separator.h \
    src/headers/add_command.h \
    src/headers/remove_command.h \
    src/headers/custom_path_item.h \

FORMS += \
    src/forms/mainwindow.ui

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
