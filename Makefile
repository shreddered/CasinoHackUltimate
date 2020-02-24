CC=g++-9
LIBS=-llept -ltesseract -lcurl 
CPPFLAGS=-O --std=c++17 -fmax-errors=10
LDFLAGS =
INCLUDE_PATH=-Iinclude/
BIN=casino_hack
# because of header-only style
MAINFILE=src/main.cpp

ifndef ($(OS))
	# unix case
	ifeq ($(shell uname -s), Darwin)
		# path to leptonica headers installed by brew
		INCLUDE_PATH += -I/usr/local/Cellar/leptonica/1.78.0_1/include/
		INCLUDE_PATH += -I/usr/local/Cellar/tesseract/4.1.0/include
		LDFLAGS += -L/usr/local/Cellar/leptonica/1.78.0_1/lib/ -L/usr/local/Cellar/tesseract/4.1.0/lib/
	endif
endif

all: main
	$(CC) $(LIBS) $(LDFLAGS) main.o -o $(BIN)
	rm main.o
main: $(MAINFILE)
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) -c $(MAINFILE) -o main.o
