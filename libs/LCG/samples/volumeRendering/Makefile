# General Makefile
# Date: 28/07/2006
# Ricardo Farias: rfarias@ufrj.br


#PATH = ./

CXX = g++
CC =  gcc

EIGENDIR = ../../../eigen
ANTDIR = ../../../AntTweakBar
GLEWDIR = ../../../glew
GLFWDIR = ../../../glfw
SHADERLIBDIR = ../../

EXECUTABLE = volumeRendering

SRC_DIR := ../../shaderlib .
BUILD_DIR := ../../objs

SRC       := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.cpp))

OBJS       := $(patsubst %.cpp, %.o, $(SRC))

INCLUDES  := $(addprefix -I,$(SRC_DIR)) -I$(EIGENDIR) -I$(ANTDIR)/include -I$(GLEWDIR)/include -I$(SHADERLIBDIR) -I$(GLFWDIR)/include

LIBDIRS  = -L$(ANTDIR)/lib -L$(GLEWDIR)/lib -L$(GLFWDIR)/lib/x11
LIBS     = -lGL -lGLEW -lGLU -lglfw -lXrandr -lAntTweakBar

CXXFLAGS    = -g -O3 $(INCLUDES) $(LIBDIRS) -D_BSD_SOURCE -fexpensive-optimizations -Wno-deprecated

vpath %.cpp $(SRC_DIR)

.PHONY: all 
all:  checkdirs samples

samples: $(OBJS)
	@echo "compiling..." $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBS)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

#clean:
#	@rm -rf $(BUILD_DIR)

clean:
	rm -f *.o *~ .depend $(EXECUTABLE)

#$(EXECUTABLE): $(OBJS)
#	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBS)

#depend:
#	$(CC) $(CXXFLAGS) -M *.cc > .depend

#clean:
#	rm -f *.o *~ .depend $(EXECUTABLE)

#all: clean depend $(EXECUTABLE)

#ifeq (.depend,$(wildcard .depend))
#include .depend
#endif
