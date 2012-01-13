CC=gcc

TARGET = example
CFLAGS = -Wall
#DEPS = hellomake.h
LDFLAGS = -lm -I.
OBJ = example.o\
	artnet_send.o
	
_all: all

all: $(TARGET)
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(TARGET)
