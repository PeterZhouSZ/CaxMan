QT            += core gui opengl xml widgets
TEMPLATE       = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -fext-numeric-literals

QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp

INCLUDEPATH += /media/daniela/Shared/Devel/lib

DEFINES     += CAXLIB
INCLUDEPATH += /media/daniela/Shared/Devel/lib/caxlib

#DEFINES     += CINOLIB
#INCLUDEPATH  += /media/daniela/Shared/Devel/lib/cinolib

DEFINES     += TINYXML2
INCLUDEPATH += /media/daniela/Shared/Devel/lib/tinyxml2
LIBS        += -ltinyxml2

DEFINES     += ZLIB
INCLUDEPATH += /media/daniela/Shared/Devel/lib/zlib-1.2.8

DEFINES     += LIBZIP
INCLUDEPATH += /media/daniela/Shared/Devel/lib/libzip-1.1.3/lib
LIBS        += -lzip -lz

DEFINES     += CGALLIB
LIBS        += -lCGAL -lgmp

DEFINES += CINOLIB_USES_TETGEN
DEFINES += TETLIBRARY
INCLUDEPATH += /media/daniela/Shared/Devel/lib/tetgen1.5.0
LIBS += -L/media/daniela/Shared/Devel/lib/tetgen1.5.0/build -ltet

DEFINES += CINOLIB_USES_TRIANGLE
INCLUDEPATH += /media/daniela/Shared/Devel/lib/Triangle
LIBS += /media/daniela/Shared/Devel/lib/Triangle/triangle.o

INCLUDEPATH += /media/daniela/Shared/Devel/lib/qhull/src/libqhullcpp
LIBS += -L/media/daniela/Shared/Devel/lib/qhull/lib -lqhullcpp -lqhullstatic_r

INCLUDEPATH  += /media/daniela/Shared/Devel/lib/libQGLViewer-2.6.3/QGLViewer
LIBS         += -L/media/daniela/Shared/Devel/lib/libQGLViewer-2.6.3/QGLViewer -lQGLViewer

INCLUDEPATH += /media/daniela/Shared/Devel/lib/shapelib-1.3.0
LIBS        += -L/media/daniela/Shared/Devel/lib/shapelib-1.3.0 -lshp

DEFINES     += GEOMETRICTOOLS
INCLUDEPATH += /media/daniela/Shared/Devel/lib/GeometricTools/GTEngine/Include
LIBS        += -L/media/daniela/Shared/Devel/lib/GeometricTools/GTEngine/lib/Release -lgtengine

DEFINES     += BOOSTLIB
LIBS        += -lboost_program_options

SOURCES += \
    main.cpp \
    src/eps3Dshape.cpp \
    src/geometry.cpp \
    src/sphere.cpp \
    src/wrapper_tetgen.cpp \
    src/wrapper_gte.cpp \
    src/eps3Dshape_edit.cpp \
    src/eps_trimesh.cpp \
    src/wrapper_sdf.cpp

HEADERS += \
    intersections.h \
    include/eps3Dshape.h \
    include/epsshape.h \
    include/geometry.h \
    include/sphere.h \
    include/dani_inline.h \
    include/danilib.h \
    src/gte_wrap.h \
    include/wrapper_gte.h \
    include/wrapper_tetgen.h \
    include/eps_trimesh.h \
    include/wrapper_sdf.h
