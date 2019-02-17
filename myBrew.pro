#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T12:54:34
#
#-------------------------------------------------

QT       += core gui printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myBrew
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    SensorWorkerThread.cpp \
    PIDThread.cpp \
    beerxmlrecipe.cpp \
    pugixml.cpp \
    vessel.cpp \
    mlt.cpp \
    kettle.cpp \
    step.cpp \
    hlt.cpp \
    hop.cpp \
    fermentable.cpp \
    brewconstantspopup.cpp \
    brewstepdurationpopup.cpp \
    tempprobecalibrationpopup.cpp \
    hltpidsettingspopup.cpp \
    bkpidsettingspopup.cpp \
    audio_alarmsettingspopup.cpp \
    brewerysettings.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    SensorWorkerThread.h \
    PIDThread.h \
    beerxmlrecipe.h \
    pugixml.hpp \
    pugiconfig.hpp \
    vessel.h \
    mlt.h \
    kettle.h \
    defines.h \
    step.h \
    hlt.h \
    hop.h \
    fermentable.h \
    brewconstantspopup.h \
    brewstepdurationpopup.h \
    tempprobecalibrationpopup.h \
    hltpidsettingspopup.h \
    bkpidsettingspopup.h \
    audio_alarmsettingspopup.h \
    brewerysettings.h

FORMS    += mainwindow.ui \
    brewconstantspopup.ui \
    brewstepdurationpopup.ui \
    tempprobecalibrationpopup.ui \
    hltpidsettingspopup.ui \
    bkpidsettingspopup.ui \
    audio_alarmsettingspopup.ui
