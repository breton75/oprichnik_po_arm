#-------------------------------------------------
#
# Project created by QtCreator 2016-11-09T09:27:41
#
#-------------------------------------------------

QT       += core gui sql svg xml serialport widgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = oprichnik_po_arm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    reportwindow.cpp \
    reportsetup.cpp \
    fuelmove.cpp \
    checkform.cpp \
    eventslog.cpp

HEADERS  += mainwindow.h \
    reportwindow.h \
    reportsetup.h \
    fuelmove.h \
    checkform.h \
    eventslog.h

FORMS    += mainwindow.ui \
    reportwindow.ui \
    reportsetup.ui \
    fuelmove.ui \
    checkform.ui \
    eventslog.ui

RESOURCES += \
    arm_res_files.qrc
