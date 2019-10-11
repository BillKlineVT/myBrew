#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T12:54:34
#
#-------------------------------------------------

QT       += core gui printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myBrew
TEMPLATE = app

INCLUDEPATH += src/ include/pugixml include/openbrew include/qcustomplot

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
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

HEADERS  += src/mainwindow.h \
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
    src/brewerysettings.h

FORMS    += src/mainwindow.ui \
    src/brewconstantspopup.ui \
    src/brewstepdurationpopup.ui \
    src/tempprobecalibrationpopup.ui \
    src/hltpidsettingspopup.ui \
    src/bkpidsettingspopup.ui \
    src/audio_alarmsettingspopup.ui
