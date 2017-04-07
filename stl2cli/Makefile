
####### To compile:

####### make LIB_DIR=/path/third/party/libs CAXLIB_DIR=/path/caxlib

#LIB_DIR = /media/daniela/Shared/Devel/lib/
#CAXLIB_DIR = $(LIB_DIR)caxlib
IMATISTL_DIR = $(LIB_DIR)ImatiSTL-4.2
LIBZIP_DIR = $(LIB_DIR)
TETGEN_DIR = $(LIB_DIR)tetgen1.5.0
TINYXML2_DIR = $(LIB_DIR)
ZLIB_DIR = $(LIB_DIR)
LIBZIP_DIR = $(LIB_DIR)/libzip-1.1.3/lib

CC= g++
RM= rm
TAR= tar

FLAGS = -std=c++11 -DIS64BITPLATFORM -DEXTENSIBLE_TMESH -DCAXLIB -DTETLIBRARY
CFLAGS += -Wall -fpermissive
CFLAGS += -I./ -I$(ROOT) -I$(LIB_DIR) -I$(CAXLIB_DIR) -I$(LIBZIP_DIR) -I$(TETGEN_DIR) -I$(TINYXML2_DIR) -I$(ZLIB_DIR)
CFLAGS += -I$(IMATISTL_DIR)/include/ImatiSTL -I$(IMATISTL_DIR)/include/Kernel -I$(IMATISTL_DIR)/include/TMesh

LIBS += -ltinyxml2 -lz -lzip
LIBS += -L$(IMATISTL_DIR)/build/ -limatistl

SOURCES.C    = stl2cli.cpp

OBJECTS  =  $(SOURCES.C:.C=.o)

EXECUTABLES  =  ./build/stl2cli

#----------


%.o:	%.C

	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<



$(EXECUTABLES): $(OBJECTS)

	$(CC) $(FLAGS) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLES) $(LIBS)



clean :
	$(RM) -f *.o


backup :
	$(RM) -f backup.tgz
	$(TAR) zcfv backup.tgz $(SOURCES.C) $(INCLUDES) Makefile
