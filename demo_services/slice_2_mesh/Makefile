PROGRAM_NAME := slice2mesh_service
SOURCES      := $(wildcard *.cpp)
OBJS         := ${SOURCES:.cpp=.o}

CPPFLAGS += -std=c++11
CPPFLAGS += -I/Users/cino/Documents/research/devel/lib/CinoLib/external/eigen
CPPFLAGS += -I/Users/cino/Documents/research/devel/lib/CinoLib/include

CPPFLAGS += -DCINOLIB_USES_BOOST
CPPFLAGS += -I/usr/local/include

CPPFLAGS += -DCINOLIB_USES_TETGEN
CPPFLAGS += -DTETLIBRARY
LDFLAGS  += -L/usr/local/lib 
LDFLAGS  += -ltet

CPPFLAGS += -DCINOLIB_USES_TRIANGLE
LDFLAGS  += -L/usr/local/lib 
LDFLAGS  += -ltriangle

.PHONY: all clean distclean

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(OBJS) 
	$(LINK.cc) $(OBJS) -o $(PROGRAM_NAME)

clean:
	@- $(RM) $(OBJS)

distclean: clean