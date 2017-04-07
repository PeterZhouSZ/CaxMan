TARGET   = stl2cli
TEMPLATE = app

DEFINES += IS64BITPLATFORM EXTENSIBLE_TMESH

INCLUDEPATH += /media/daniela/Shared/Devel/lib
INCLUDEPATH += /media/daniela/Shared/Devel/lib/caxlib

INCLUDEPATH += /media/daniela/Shared/Devel/lib/ImatiSTL-4.2/include/ImatiSTL
INCLUDEPATH += /media/daniela/Shared/Devel/lib/ImatiSTL-4.2/include/Kernel
INCLUDEPATH += /media/daniela/Shared/Devel/lib/ImatiSTL-4.2/include/TMesh
LIBS    += -L/media/daniela/Shared/Devel/lib/ImatiSTL-4.2/build -limatistl

INCLUDEPATH += /media/daniela/Shared/Devel/lib/tinyxml2
INCLUDEPATH += /media/daniela/Shared/Devel/lib/libzip-1.1.3/lib
LIBS += -ltinyxml2
LIBS += -lzip -lz

SOURCES += \
    stl2cli.cpp
