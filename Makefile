CC=g++
CFLAGS=-c -Wall -std=c++20
LDFLAGS=

SOURCES=ipkcpc.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ipkcpc

.PHONY: all clean run

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

run:
	./$(EXECUTABLE)