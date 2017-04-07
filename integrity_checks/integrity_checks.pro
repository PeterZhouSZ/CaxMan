QT += core
QT -= gui

CONFIG += c++11

TARGET = geometric_integrity_checks
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

DEFINES += IS64BITPLATFORM EXTENSIBLE_TMESH CAXLIB

SOURCES += main.cpp \
    checks.cpp

INCLUDEPATH += /media/daniela/Shared/Devel/lib/ImatiSTL-4.2/include/ImatiSTL
INCLUDEPATH += /media/daniela/Shared/Devel/lib/ImatiSTL-4.2/include/Kernel
INCLUDEPATH += /media/daniela/Shared/Devel/lib/ImatiSTL-4.2/include/TMesh
LIBS    += -L/media/daniela/Shared/Devel/lib/ImatiSTL-4.2/build -limatistl

INCLUDEPATH += /media/daniela/Shared/Devel/lib
INCLUDEPATH += /media/daniela/Shared/Devel/lib/caxlib
INCLUDEPATH += /media/daniela/Shared/Devel/lib/tinyxml2
INCLUDEPATH += /media/daniela/Shared/Devel/lib/libzip-1.1.3/lib
LIBS += -ltinyxml2
LIBS += -lzip -lz

HEADERS += \
    checks.h


