CC = g++-9
LIBS = -llept -ltesseract -lcurl 
CPPFLAGS= -O --std=c++17 
LDFLAGS =
INCLUDE_PATH = -Iinclude/
BIN = casino_hack
# because of header-only style
MAINFILE=src/main.cpp
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:src/%.cpp=bin/%.o)

ifndef ($(OS))
	# unix case
	ifeq ($(shell uname -s), Darwin)
		# path to leptonica headers installed by brew
		INCLUDE_PATH += -I/usr/local/Cellar/leptonica/1.78.0_1/include/
		INCLUDE_PATH += -I/usr/local/Cellar/tesseract/4.1.0/include
		LDFLAGS += -L/usr/local/Cellar/leptonica/1.78.0_1/lib/ -L/usr/local/Cellar/tesseract/4.1.0/lib/
	endif
endif

all: $(OBJECTS)
	@echo "Linking... "
	@$(CC) $(LIBS) $(LDFLAGS) $(OBJECTS) -o $(BIN)
	@echo "done"
	@echo "Deleting temporary objects... "
	@rm $(OBJECTS)
	@echo "done"

bin/%.o: src/%.cpp
	@echo "Building src/$<..."
	@$(CC) $(INCLUDE_PATH) $(CPPFLAGS) -c $< -o $@ 
	@echo " done"
