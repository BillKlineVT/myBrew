include(gtest_dependency.pri)

QT       += core gui printsupport sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = myBrew_test
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt

INCLUDEPATH += ../src/ ../include/qcustomplot ../include/pugixml ../include/openbrew

HEADERS += \
        ../src/mainwindow.h \
        ../include/qcustomplot/qcustomplot.h \
        ../src/SensorWorkerThread.h \
        ../src/PIDThread.h \
        ../src/beerxmlrecipe.h \
        ../include/pugixml/pugixml.hpp \
        ../include/pugixml/pugiconfig.hpp \
        ../include/openbrew/vessel.h \
        ../include/openbrew/mlt.h \
        ../include/openbrew/kettle.h \
        ../include/openbrew/defines.h \
        ../include/openbrew/step.h \
        ../include/openbrew/hlt.h \
        ../include/openbrew/hop.h \
        ../include/openbrew/fermentable.h \
        ../src/brewconstantspopup.h \
        ../src/brewstepdurationpopup.h \
        ../src/tempprobecalibrationpopup.h \
        ../src/hltpidsettingspopup.h \
        ../src/bkpidsettingspopup.h \
        ../src/audio_alarmsettingspopup.h \
        ../src/brewerysettings.h

SOURCES += \
        main.cpp \
        myBrew_test.cpp \
        ../src/mainwindow.cpp \
        ../include/qcustomplot/qcustomplot.cpp \
        ../src/SensorWorkerThread.cpp \
        ../src/PIDThread.cpp \
        ../src/beerxmlrecipe.cpp \
        ../include/pugixml/pugixml.cpp \
        ../include/openbrew/vessel.cpp \
        ../include/openbrew/mlt.cpp \
        ../include/openbrew/kettle.cpp \
        ../include/openbrew/step.cpp \
        ../include/openbrew/hlt.cpp \
        ../include/openbrew/hop.cpp \
        ../include/openbrew/fermentable.cpp \
        ../src/brewconstantspopup.cpp \
        ../src/brewstepdurationpopup.cpp \
        ../src/tempprobecalibrationpopup.cpp \
        ../src/hltpidsettingspopup.cpp \
        ../src/bkpidsettingspopup.cpp \
        ../src/audio_alarmsettingspopup.cpp \
        ../src/brewerysettings.cpp

FORMS    += ../src/mainwindow.ui \
    ../src/brewconstantspopup.ui \
    ../src/brewstepdurationpopup.ui \
    ../src/tempprobecalibrationpopup.ui \
    ../src/hltpidsettingspopup.ui \
    ../src/bkpidsettingspopup.ui \
    ../src/audio_alarmsettingspopup.ui