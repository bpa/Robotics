LIBS = -lm
CC = gcc
CFLAGS = -g -Wall -fPIC
LDFLAGS = -shared

TARGET = libsolv.so
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

all: $(TARGET)

%.o: %.c $(HEADERS)
	    $(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	    $(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@ $(OBJECTS)

clean:
	    -rm -f *.o
	    -rm -f $(TARGET)
