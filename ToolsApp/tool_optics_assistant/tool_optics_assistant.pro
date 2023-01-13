QT += core gui widgets xml concurrent
TARGET = tool_optics_assistant
TEMPLATE = app
DESTDIR = /home/adt/micro-i/hmi_tool_optics_assistant
DEFINES += QT_DEPRECATED_WARNINGS

include(../../DisplayComponents/DisplayComponents.pri)
include(../../Common/Common.pri)
include(../../CameraBasics/CameraBasics.pri)
include(../../CameraModules/CameraModules.pri)
include(../../ToolsModules/ToolsModules.pri)

DEFINES += GLOBAL_HAIKANG

DEFINES += GLOBAL_DAHENG

contains(DEFINES, GLOBAL_HAIKANG){
    INCLUDEPATH += /opt/MVS/include
    LIBS +=      -L/opt/MVS/lib/64 -lMvCameraControl
}

contains(DEFINES, GLOBAL_DAHENG){
    INCLUDEPATH += /opt/Galaxy_camera/inc
    LIBS += -lgxiapi
}

contains(DEFINES, GLOBAL_DAHUA){
    INCLUDEPATH += /opt/DahuaTech/MVviewer/include
    LIBS += -L/opt/DahuaTech/MVviewer/lib -lMVSDK -lImageConvert
    LIBS += -L/opt/DahuaTech/MVviewer/lib/GenICam/bin/Linux64_x64 -lGCBase_gcc421_v3_0 -lGenApi_gcc421_v3_0 -lLog_gcc421_v3_0 -llog4cpp_gcc421_v3_0
    LIBS += -L/opt/DahuaTech/MVviewer/lib/GenICam/bin/Linux64_x64 -lNodeMapData_gcc421_v3_0 -lXmlParser_gcc421_v3_0 -lMathParser_gcc421_v3_0 -lrt -lpthread
}

contains(DEFINES, GLOBAL_DALSA_NET){
    INCLUDEPATH += /usr/dalsa/GigeV/include
    INCLUDEPATH += /usr/dalsa/GigeV/examples/common
    INCLUDEPATH += /opt/genicam_v3_0/library/CPP/include

    LIBS += /usr/local/lib/libGevApi.so.2 \
            /usr/local/lib/libCorW32.so.1 \
            /opt/genicam_v3_0/bin/Linux64_x64/libGenApi_gcc421_v3_0.so \
            /opt/genicam_v3_0/bin/Linux64_x64/libGCBase_gcc421_v3_0.so
}

contains(DEFINES, GLOBAL_DALSA_CAMERALINK){
    INCLUDEPATH += /opt/hmi_depends/DALSA/include
    INCLUDEPATH += /opt/hmi_depends/DALSA/classes/basic
    LIBS +=        /opt/hmi_depends/DALSA/lib/libSapera++.a
    LIBS +=      -L/opt/hmi_depends/DALSA/lib -lCorLog -lCorW32 -lSaperaLT
    QMAKE_CXXFLAGS = -g -DPOSIX_HOSTPC -D_REENTRANT -Dx86_64 -Wall
    QMAKE_CXXFLAGS += -Wno-unknown-pragmas
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
}

INCLUDEPATH   += /opt/hmi_depends/opencv3.4.1/include
INCLUDEPATH   += /opt/hmi_depends/opencv3.4.1/include/opencv
INCLUDEPATH   += /opt/hmi_depends/opencv3.4.1/include/opencv2

LIBS += /opt/hmi_depends/opencv3.4.1/libs/libopencv*.so.3.4.1

SOURCES += \
    CameraConfig.cpp \
    CameraFrameItem.cpp \
    CameraImageWindow.cpp \
    CameraSelect.cpp \
    main.cpp \
    MainWindow.cpp \
    ProductConfig.cpp \
    CameraTreeWidget.cpp

HEADERS += \
    CameraConfig.h \
    CameraFrameItem.h \
    CameraImageWindow.h \
    CameraSelect.h \
    MainWindow.h \
    ProductConfig.h \
    CameraTreeWidget.h

FORMS += \
    CameraConfig.ui \
    CameraFrameItem.ui \
    CameraImageWindow.ui \
    CameraSelect.ui \
    MainWindow.ui \
    ProductConfig.ui

