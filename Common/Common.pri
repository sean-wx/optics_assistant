QT += sql

INCLUDEPATH += $$PWD

INCLUDEPATH   += /opt/hmi_depends/opencv3.4.1/include
INCLUDEPATH   += /opt/hmi_depends/opencv3.4.1/include/opencv
INCLUDEPATH   += /opt/hmi_depends/opencv3.4.1/include/opencv2

#libs
LIBS += /opt/hmi_depends/opencv3.4.1/libs/libopencv*.so.3.4.1

FORMS += \
    $$PWD/ColorPalette.ui \
    $$PWD/SaveImageConfig.ui

HEADERS += \
    $$PWD/ColorPalette.h \
    $$PWD/comm_def.h \
    $$PWD/CreateXML.h \
    $$PWD/ImageDetection.h \
    $$PWD/AbstractDbClient.h \
    $$PWD/SaveImageConfig.h

SOURCES += \
    $$PWD/ColorPalette.cpp \
    $$PWD/CreateXML.cpp \
    $$PWD/ImageDetection.cpp \
    $$PWD/AbstractDbClient.cpp \
    $$PWD/SaveImageConfig.cpp

RESOURCES += \
    $$PWD/resource.qrc
