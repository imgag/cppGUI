include("../lib.pri")

#base settings
QT       += gui widgets charts
TARGET = cppGUI
DEFINES += CPPGUI_LIBRARY

#include cppCORE library
INCLUDEPATH += $$PWD/../cppCORE
LIBS += -L$$PWD/../../bin -lcppCORE

SOURCES += \
    Application.cpp \
    GUIHelper.cpp \
    FileChooser.cpp \
    ClickableLabel.cpp \
    ClickableLineEdit.cpp \
    ColorSelector.cpp \
    ScrollableTextDialog.cpp \
    BusyDialog.cpp \
    QrCodeGenerator/BitBuffer.cpp \
    QrCodeGenerator/QrCode.cpp \
    QrCodeGenerator/QrSegment.cpp \
    QrCodeFactory.cpp \
    DelayedInitializationTimer.cpp \
    TsvTableWidget.cpp \
    ImageLabel.cpp

HEADERS += \
    Application.h \
    GUIHelper.h \
    FileChooser.h \
    ClickableLabel.h \
    ClickableLineEdit.h \
    ColorSelector.h \
    ScrollableTextDialog.h \
    BusyDialog.h \
    QrCodeGenerator/BitBuffer.h \
    QrCodeGenerator/QrCode.h \
    QrCodeGenerator/QrSegment.h \
    QrCodeFactory.h \
    DelayedInitializationTimer.h \
    TsvTableWidget.h \
    ImageLabel.h
    
RESOURCES += \
    resources.qrc

FORMS += \
    ScrollableTextDialog.ui \
    BusyDialog.ui \
    TsvTableWidget.ui
