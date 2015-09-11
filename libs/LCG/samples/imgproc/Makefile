# General Makefile

#PATH = ./

CXX = g++
CC =  gcc

EIGENDIR = ../../../eigen
ANTDIR = ../../../AntTweakBar
GLEWDIR = ../../../glew
SHADERLIBDIR = ../../

EXECUTABLE = imgproc

SRC_DIR := ../../shaderlib .
BUILD_DIR := ../../objs

SRC       := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.cpp))

OBJS       := $(patsubst %.cpp, %.o, $(SRC))
#OBJS       := $(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(SRC)))

INCLUDES  := $(addprefix -I,$(SRC_DIR)) -I$(EIGENDIR) -I$(ANTDIR)/include  -I$(GLEDIR)/include -I$(SHADERLIBDIR)

LIBDIRS  = -L$(ANTDIR)/lib -L$(GLEWDIR)/lib
LIBS     = -lGL -lGLEW -lGLU -lglfw -lXrandr -lAntTweakBar -lX11

CXXFLAGS    = -g -O3 $(INCLUDES) $(LIBDIRS) -D_BSD_SOURCE -fexpensive-optimizations -Wno-deprecated

vpath %.cpp $(SRC_DIR)

# define make-goal
# $1/%.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBS)
#     #$(CC) $(INCLUDES) -c $$< -o $$@
# endef

.PHONY: all 
all:  checkdirs samples

samples: $(OBJS)
	@echo "compiling..." $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE) $(OBJS) $(LIBS)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR) *.o imgproc

