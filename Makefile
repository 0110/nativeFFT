CC=g++

#CFLAGS = -I.
#DEPS = hellomake.h
LDFLAGS = -lopenal -lfftw3 -lm -lncurses
OBJ = example.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
