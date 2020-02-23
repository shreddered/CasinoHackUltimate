CC=g++
LIBS=-llept -ltesseract -lcurl 
CPPFLAGS=-O --std=c++17
INCLUDE_PATH=-Iinclude/
BIN=test
# because of header-only style
MAINFILE=src/main.cpp

ifndef ($(OS))
	# unix case
	ifeq ($(shell uname -s), Darwin)
		# path to leptonica headers installed by brew
		INCLUDE_PATH += -I/usr/local/Cellar/leptonica/1.78.0/include/
	endif
endif

all: main
	$(CC) $(LIBS) main.o -o $(BIN)
	rm main.o
main: $(MAINFILE)
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) -c $(MAINFILE) -o main.o
