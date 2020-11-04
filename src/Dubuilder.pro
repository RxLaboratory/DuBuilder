#-------------------------------------------------
#
# Project created by QtCreator 2017-09-22T09:46:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dubuilder
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    scanner.cpp \
    script.cpp \
    scriptwidget.cpp \
    settingswidget.cpp \
    toolbarspacer.cpp \
    builder.cpp \
    frameless.cpp \
    rainboxui.cpp

HEADERS  += mainwindow.h \
    scanner.h \
    script.h \
    scriptwidget.h \
    settingswidget.h \
    toolbarspacer.h \
    builder.h \
    frameless.h \
    rainboxui.h \
    version.h

FORMS    += mainwindow.ui \
    scriptwidget.ui \
    settingswidget.ui \
    toolbarspacer.ui

RESOURCES += \
    resources.qrc


# OS Specific configurations
win* {
    # Add version and other metadata
    DISTFILES += app.rc
    RC_FILE = app.rc
    !build_pass:touch($$RC_FILE, version.h)
    # Enable console output
    CONFIG += console
} else:unix {
    # Fix issue with c++ version used to compile Qt in some distros (Ubuntu) with Qt <= 5.12.
    # Need to check the version of c++ used with distros providing Qt > 12
    equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 13):QMAKE_CXXFLAGS += "-fno-sized-deallocation"
} else:macx {

}

ICON = resources/icons/appIcon.icns
DISTFILES += \
    app.rc
