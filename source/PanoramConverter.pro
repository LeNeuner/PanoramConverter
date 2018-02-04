#-------------------------------------------------
#
# Project created by QtCreator 2018-01-31T16:54:15
#
#-------------------------------------------------

QT       += core gui widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG          += c++11
QMAKE_CXXFLAGS  += -std=c++11
QMAKE_LFLAGS    += -fopenmp

#TARGET   = PanoramConverter
TEMPLATE = app

CONFIG (debug, debug|release) {
    TARGET      = PanoramConverter_d
    DESTDIR     = "../debug"
    OBJECTS_DIR = "../Converter_debug/"$$TARGET"/Objects"
    MOC_DIR     = "../Converter_debug/"$$TARGET"/MOCs"
    RCC_DIR     = "../Converter_debug/"$$TARGET"/RCCs"
    UI_DIR      = "../Converter_debug/"$$TARGET"/UIs"
} else {
    TARGET      = PanoramConverter
    DESTDIR     = "../build"
    OBJECTS_DIR = "../Converter_build/"$$TARGET"/Objects"
    MOC_DIR     = "../Converter_build/"$$TARGET"/MOCs"
    RCC_DIR     = "../Converter_build/"$$TARGET"/RCCs"
    UI_DIR      = "../Converter_build/"$$TARGET"/UIs"
}

INCLUDEPATH     += "../build"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=  main.cpp                \
            convertmainwindow.cpp   \
            imageconverter.cpp

HEADERS +=  convertmainwindow.h     \
            imageconverter.h

FORMS   +=  convertmainwindow.ui

RESOURCES += icons/iconres.qrc

