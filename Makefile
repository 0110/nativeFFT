CC=g++

TARGET = example
CFLAGS = -Wall
#DEPS = hellomake.h
LDFLAGS = -lopenal -lfftw3 -lm -lncurses
OBJ = example.o
_all: all

all: $(TARGET)
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(TARGET)
