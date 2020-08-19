CC=gcc
SRC=dartlogrdr.c FileBuffer.c DartLogParser.c Slice.c ../server_objects/databuf.c
OBJ=$(SRC:.c=.o)
LIB=-ldl
INC=-I..
CFLAGS=-g -fPIC

all: dartlogrdr

dartlogrdr: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIB)

%.o: %.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f dartlogrdr
