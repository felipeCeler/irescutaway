CC = g++
CFLAGS_DEBUG = -Wall -Wextra -pedantic -g3 -std=c++0x -O0
# If you get weird bugs, use -O2 instead of -O3
CFLAGS_RELEASE = -O2 -std=c++0x

ODIR = obj
LIBDIR = lib
LIB_SDIR = ires
TEST_SDIR = test
DYANMICPROPGEN_SDIR = dynamicPropGen
FACEPROPGEN_SDIR = faceProperGen
OLIB = $(LIBDIR)/libires.a
EXEDIR = exe
OTEST = $(EXEDIR)/test
ODYANMICPROPGEN = $(EXEDIR)/dynamicPropGen
OFACEPROPGEN = $(EXEDIR)/faceCornerEdgeGen

CFLAGS = $(CFLAGS_DEBUG)

_LIB_OBJS = ires.o kdtreeires.o reader.o writer.o \
		snappy.o snappy-c.o snappy-sinksource.o \
		snappy-stubs-internal.o
LIB_OBJS = $(patsubst %,$(ODIR)/%,$(_LIB_OBJS))

_TEST_OBJS = test.o
TEST_OBJS = $(patsubst %,$(ODIR)/%,$(_TEST_OBJS))

_DYANMICPROPGEN_OBJS = dynamicPropGen.o
DYANMICPROPGEN_OBJS = $(patsubst %,$(ODIR)/%,$(_DYANMICPROPGEN_OBJS))

_FACEPROPGEN_OBJS = faceCornerEdgeGen.o
FACEPROPGEN_OBJS = $(patsubst %,$(ODIR)/%,$(_FACEPROPGEN_OBJS))

$(ODIR)/%.o: $(LIB_SDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(LIB_SDIR)/snappy/%.cc
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(TEST_SDIR)/%.cpp
	$(CC) -c -I$(LIB_SDIR) -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(DYANMICPROPGEN_SDIR)/%.cpp
	$(CC) -c -I$(LIB_SDIR) -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(FACEPROPGEN_SDIR)/%.cpp
	$(CC) -c -I$(LIB_SDIR) -o $@ $< $(CFLAGS)

all: makedirs test dynamicPropGen faceCornerEdgeGen
	@echo
	@echo =========================================================
	@echo \ \ \ \ NOTE: Type \'make release\' to build in RELEASE mode.
	@echo =========================================================

makedirs:
	mkdir -p $(LIBDIR)
	mkdir -p $(ODIR)
	mkdir -p $(EXEDIR)

ires: ires/ires.h ires/primitivesires.h ires/kdtreeires.h $(LIB_OBJS)
	ar rvs $(OLIB) $^

dynamicPropGen: ires $(DYANMICPROPGEN_OBJS)
	$(CC) $(CFLAGS) -I$(LIBDIR) $(DYANMICPROPGEN_OBJS) -L$(LIBDIR) -lires -o $(ODYANMICPROPGEN)

faceCornerEdgeGen: ires $(FACEPROPGEN_OBJS)
	$(CC) $(CFLAGS) -I$(LIBDIR) $(FACEPROPGEN_OBJS) -L$(LIBDIR) -lires -o $(OFACEPROPGEN)

test: ires $(TEST_OBJS)
	$(CC) $(CFLAGS) -I$(LIBDIR) $(TEST_OBJS) -L$(LIBDIR) -lires -o $(OTEST)

.PHONY: release
release:
	make clean
	make CFLAGS='$(CFLAGS_RELEASE)'

.PHONY: clean
clean:
	rm -rf $(OLIB) $(OTEST) $(ODIR)/*.o $(EXEDIR)/*

