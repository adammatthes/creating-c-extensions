CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -g -I../
LDFLAGS=$(shell pkg-config --cflags --libs check)

# Source files
SOURCES=test_fib_heap.c ../fibonacci_heap.c

# Object files
OBJECTS=$(SOURCES:.c=.o)

# Executable name
TARGET=test_runner

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: all
	./$(TARGET)
