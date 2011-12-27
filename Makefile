CC=g++

#CFLAGS = -I.
#DEPS = hellomake.h
LDFLAGS = -lopenal
OBJ = example.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
