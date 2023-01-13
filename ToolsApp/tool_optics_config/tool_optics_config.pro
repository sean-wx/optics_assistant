QT += core gui widgets xml concurrent

TEMPLATE = app
TARGET = tool_optics_config
DESTDIR = /home/adt/micro-i/hmi_tool_optics_assistant

DEFINES += QT_DEPRECATED_WARNINGS

include(../../DisplayComponents/DisplayComponents.pri)
include(../../Common/Common.pri)
include(../../ToolsModules/ToolsModules.pri)

SOURCES += main.cpp
