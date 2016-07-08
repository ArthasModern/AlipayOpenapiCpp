TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/libs/include
DEPENDPATH += $$PWD/libs

LIBS += -L$$PWD/libs
LIBS += -llibeay32
LIBS += -lssleay32
LIBS += -llibcurl

QMAKE_CFLAGS_DEBUG += -MT
QMAKE_CXXFLAGS_DEBUG += -MT
QMAKE_CFLAGS_RELEASE += -MT
QMAKE_CXXFLAGS_RELEASE += -MT

QMAKE_LFLAGS_DEBUG   = /DEBUG /NODEFAULTLIB:libcmt.lib
QMAKE_LFLAGS_RELEASE = /RELEASE /NODEFAULTLIB:libcmt.lib

SOURCES += main.cpp \
    openapi/openapi_client.cpp \
    openapi/http/http_client.cpp \
    openapi/json/cJSON.c \
    openapi/json/json_util.cpp \
    openapi/json/json_type.cpp

HEADERS += \
    openapi/openapi_client.h \
    openapi/http/http_client.h \
    openapi/json/cJSON.h \
    openapi/json/json_util.h \
    openapi/json/json_type.h

DEFINES += CURL_STATICLIB
