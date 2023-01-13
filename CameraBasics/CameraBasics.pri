#
# 本工程包含了各种相机的基础控制类
#
INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/DahengUsbAreaCamera.h \
    $$PWD/DahengNetAreaCamera.h \
    $$PWD/HaikangNetAreaCamera.h \
    $$PWD/HaikangNetLineScanCamera.h \
    $$PWD/HaikangNetCamera.h \
    $$PWD/DahengAreaCamera.h

SOURCES += \
    $$PWD/DahengUsbAreaCamera.cpp \
    $$PWD/DahengNetAreaCamera.cpp \
    $$PWD/HaikangNetAreaCamera.cpp \
    $$PWD/HaikangNetLineScanCamera.cpp \
    $$PWD/HaikangNetCamera.cpp \
    $$PWD/DahengAreaCamera.cpp

