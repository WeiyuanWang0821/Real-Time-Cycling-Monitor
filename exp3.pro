########################################
# File: exp3.pro
# Description: Qt project configuration file for a GUI app
#              Includes serial communication, chart plotting,
#              and external sensor libraries 
########################################
QT       += core gui serialport charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Load libraries and include paths
LIBS += -lwiringPi
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    o2.cpp \
    scd4x.cpp \
    sfm3300.cpp \
    widget.cpp

HEADERS += \
    o2.h \
    scd4x.h \
    sfm3300.h \
    widget.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    changelog.txt
