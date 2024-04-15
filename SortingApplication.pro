QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 qt

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    custom.cpp \
    testingsorts.cpp \
    sortvisualization.cpp

HEADERS += \
    mainwindow.h \
    DynamicArray.h \
    LinkedList.h \
    Sequence.h \
    sorts.hpp \
    custom.h \
    testingsorts.h \
    sortvisualization.h

FORMS += \
    mainwindow.ui \
    custom.ui \
    testingsorts.ui \
    sortvisualization.ui

TRANSLATIONS += \
    SortingApplication_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

win32: RC_FILE = file.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
