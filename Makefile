CC=g++
LIBS=-llept -ltesseract -lcurl 
CPPFLAGS=-O --std=c++17
INCLUDE_PATH=-I/usr/local/Cellar/leptonica/1.78.0/include/ -I./src/include/
BIN=test
# because of header-only style
MAINFILE=src/main.cpp

all: main
	$(CC) $(LIBS) main.o -o $(BIN)
	rm main.o
main: $(MAINFILE)
	$(CC) $(CPPFLAGS) $(INCLUDE_PATH) -c $(MAINFILE) -o main.o
