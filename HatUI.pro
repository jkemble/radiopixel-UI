#-------------------------------------------------
#
# Project created by QtCreator 2015-09-29T18:56:58
#
#-------------------------------------------------

QT       += core gui bluetooth

//macx {
//QT       += serialport
//DEFINES  += SERIAL
//}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HatUI
TEMPLATE = app

INCLUDEPATH += /Users/blynch/Documents/Arduino/libraries

SOURCES += main.cpp\
        mainwindow.cpp \
    /Users/blynch/Documents/Arduino/libraries/radiopixel-protocol/radiopixel_protocol.cpp \
    colorpicker.cpp

HEADERS  += mainwindow.h \
    colorpicker.h

RESOURCES += \
    resources.qrc

ICON = hat.icns

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
