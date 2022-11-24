CC=gcc
OBJFILES=$(wildcard ./*.c)
TARGET=example

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) -I../src -I../deps $(OBJFILES) -o $(TARGET) -L../ -lutil

clean:
	rm $(TARGET)
