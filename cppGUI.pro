include("../lib.pri")

#base settings
QT       += gui widgets charts
TARGET = cppGUI
DEFINES += CPPGUI_LIBRARY

#include cppCORE library
INCLUDEPATH += $$PWD/../cppCORE
LIBS += -L$$PWD/../../bin -lcppCORE

SOURCES += \
    AboutDialog.cpp \
    Application.cpp \
    GUIHelper.cpp \
    ClickableLabel.cpp \
    ClickableLineEdit.cpp \
    ColorSelector.cpp \
    MarkdownEditor.cpp \
    ParameterEditor.cpp \
    ScrollableTextDialog.cpp \
    BusyDialog.cpp \
    DelayedInitializationTimer.cpp \
    TsvTableWidget.cpp \
    ImageLabel.cpp

HEADERS += \
    AboutDialog.h \
    Application.h \
    GUIHelper.h \
    ClickableLabel.h \
    ClickableLineEdit.h \
    ColorSelector.h \
    MarkdownEditor.h \
    ParameterEditor.h \
    ScrollableTextDialog.h \
    BusyDialog.h \
    DelayedInitializationTimer.h \
    TsvTableWidget.h \
    ImageLabel.h
    
RESOURCES += \
    resources.qrc

FORMS += \
    AboutDialog.ui \
    MarkdownEditor.ui \
    ParameterEditor.ui \
    ScrollableTextDialog.ui \
    BusyDialog.ui \
    TsvTableWidget.ui
