CC=g++
CFLAGS=-I.
LIBS = `pkg-config --cflags --libs opencv4`
LDLIBS = -ltiff -lpng -ljpeg -lz -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_objdetect

IDIR = ../include
_DEPS = feature_extract.h csv_util.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))


EXT = extraction.cpp feature_extract.o csv_util.o
#OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

MAT = CBIR.cpp feature_extract.o csv_util.o
#OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

#SRC = extraction.cpp 

all: extract matching

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(LIBS)

extract: $(EXT)
	$(CC) -o $@ $^ $(LIBS)

matching: $(MAT)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm -f *.o *~
