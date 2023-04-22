CC=gcc
CCFLAGS=-g -Wall
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=scheduling

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS) 
%.o: %.c %.h
		$(CC) $(CCFLAGS) -c $<
.o: %.c
		$(CC) $(CCFLAGS) -c $<
clean:
	        rm -f *.o $(TARGET)