QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = yarge2.ico

VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    about.cpp \
    hash.cpp \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    outputdialog.cpp \
    tab.cpp \
    yarge_yara.cpp

HEADERS += \
    about.h \
    hash.h \
    mainwindow.h \
    codeeditor.h \
    outputdialog.h \
    tab.h \
    yarge_yara.h

FORMS += \
    about.ui \
    mainwindow.ui \
    outputdialog.ui

TRANSLATIONS += \
    ts/yarge_en_US.ts \
    ts/yarge_pt_BR.ts
CONFIG += lrelease
CONFIG += embed_translations

LIBS += /usr/lib/libyara.so
LIBS += /usr/lib/libjansson.so
LIBS += /usr/lib/libssl.so

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc
