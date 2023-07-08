QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = yarge2.ico

VERSION = 1.5.4

DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    dialogrulesmatch.cpp \
    dialogsettings.cpp \
    hash.cpp \
    highlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    outputdialog.cpp \
    searchdialog.cpp \
    settings.cpp \
    utils.cpp \
    yarge_yara.cpp

HEADERS += \
    about.h \
    dialogrulesmatch.h \
    dialogsettings.h \
    hash.h \
    highlighter.h \
    mainwindow.h \
    codeeditor.h \
    outputdialog.h \
    searchdialog.h \
    settings.h \
    tab.h \
    utils.h \
    yarge_yara.h

FORMS += \
    about.ui \
    dialogrulesmatch.ui \
    dialogsettings.ui \
    mainwindow.ui \
    outputdialog.ui \
    searchdialog.ui

TRANSLATIONS += \
    ts/yarge_en_US.ts \
    ts/yarge_pt_BR.ts
CONFIG += lrelease
CONFIG += embed_translations

#
LIBS += /usr/lib/libyara.so
LIBS += /usr/lib/libjansson.so
LIBS += /usr/lib/libssl.so

# Windows
#INCLUDEPATH += "yara include path"
#LIBS += yara lib path/libyara.lib
#LIBS += ws2_32.lib
#LIBS += kernel32.lib
#LIBS += user32.lib
#LIBS += Advapi32.lib
#LIBS += Crypt32.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES +=
