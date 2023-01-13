#-------------------------------------------------
#
# Project created by QtCreator 2020-12-25T11:48:33
#
#-------------------------------------------------
QT       += network
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/AbstractCamera.h \
    $$PWD/TM_DahengNetAreaScanCamera.h \
    $$PWD/TM_DahengUsbAreaScanCamera.h \
    $$PWD/TM_HaikangNetAreaScanCamera.h \
    $$PWD/TM_HaikangNetLineScanCamera.h \
    $$PWD/CameraManager.h

SOURCES += \
    $$PWD/AbstractCamera.cpp \
    $$PWD/TM_DahengNetAreaScanCamera.cpp \
    $$PWD/TM_DahengUsbAreaScanCamera.cpp \
    $$PWD/TM_HaikangNetAreaScanCamera.cpp \
    $$PWD/TM_HaikangNetLineScanCamera.cpp \
    $$PWD/CameraManager.cpp
