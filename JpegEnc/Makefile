CC=g++
CFLAGS=-c -std=c++0x -O3
LDFLAGS=
LIBS=-lstdc++
DIRS=-I./include
SOURCES=\
		src/stdafx.cpp\
		src/jpeg/fdct.cpp\
		src/jpeg/huffman.cpp\
		src/jpeg/jpegfile.cpp\
		src/jpeg/quantization.cpp\
		src/util/bitvector.cpp\
		src/util/blocksplitter.cpp\
		src/util/colorutil.cpp\
		src/util/block.cpp\
		src/util/bmp.cpp\
		src/util/hufftable.cpp

MAIN=src/main.cpp

TMP=$(SOURCES:.cpp=.o)
SOURCES_O=$(TMP:.cpp=.o)

MAIN_O=$(MAIN:.cpp=.o)

EXECUTABLE=jpegenc

all: $(SOURCES) $(MAIN) $(EXECUTABLE)
$(EXECUTABLE): $(SOURCES_O) $(MAIN_O)
		$(CC) $(SOURCES_O) $(MAIN_O) $(LDFLAGS) -o $@ $(LIBS)

%.o: %.cpp
		$(CC) $(DIRS) $(CFLAGS) $< -o $@

clean:
		rm -f $(SOURCES_O) $(TESTS_O) $(MAIN_O) $(EXECUTABLE)

