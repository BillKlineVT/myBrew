#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T12:54:34
#
#-------------------------------------------------

QT       += core gui printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myBrewLib
TEMPLATE = lib

DEFINES += WIDGETSLIB_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += src/ include/pugixml include/openbrew include/qcustomplot

SOURCES += \
        src/myBrew.cpp \
    include/qcustomplot/qcustomplot.cpp \
    src/SensorWorkerThread.cpp \
    src/PIDThread.cpp \
    src/beerxmlrecipe.cpp \
    include/pugixml/pugixml.cpp \
    include/openbrew/vessel.cpp \
    include/openbrew/mlt.cpp \
    include/openbrew/kettle.cpp \
    include/openbrew/step.cpp \
    include/openbrew/hlt.cpp \
    include/openbrew/hop.cpp \
    include/openbrew/fermentable.cpp \
    src/brewconstantspopup.cpp \
    src/brewstepdurationpopup.cpp \
    src/tempprobecalibrationpopup.cpp \
    src/hltpidsettingspopup.cpp \
    src/bkpidsettingspopup.cpp \
    src/audio_alarmsettingspopup.cpp \
    src/brewerysettings.cpp

HEADERS  +=  src/myBrew.h \
    include/qcustomplot/qcustomplot.h \
    src/SensorWorkerThread.h \
    src/PIDThread.h \
    src/beerxmlrecipe.h \
    include/pugixml/pugixml.hpp \
    include/pugixml/pugiconfig.hpp \
    include/openbrew/vessel.h \
    include/openbrew/mlt.h \
    include/openbrew/kettle.h \
    include/openbrew/defines.h \
    include/openbrew/step.h \
    include/openbrew/hlt.h \
    include/openbrew/hop.h \
    include/openbrew/fermentable.h \
    src/brewconstantspopup.h \
    src/brewstepdurationpopup.h \
    src/tempprobecalibrationpopup.h \
    src/hltpidsettingspopup.h \
    src/bkpidsettingspopup.h \
    src/audio_alarmsettingspopup.h \
    src/brewerysettings.h \
    src/myBrewLibGlobal.h

FORMS    += src/myBrew.ui \
    src/brewconstantspopup.ui \
    src/brewstepdurationpopup.ui \
    src/tempprobecalibrationpopup.ui \
    src/hltpidsettingspopup.ui \
    src/bkpidsettingspopup.ui \
    src/audio_alarmsettingspopup.ui

unix {
    target.path = /usr/lib
    INSTALLS += target
}
