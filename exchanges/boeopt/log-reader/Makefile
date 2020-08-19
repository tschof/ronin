CC=gcc
CFLAGS=-g -fPIC
SRC=BATS-Binary-Textualizer.c ../../server_objects/databuf.c ../DataTypes.c ../../server_objects/tm_to_str.c
OBJ=$(SRC:.c=.o)
LIB=
INC=-I../..

all: boeopt.so

boeopt.so: $(OBJ)
	$(CC) $(OBJ) -fPIC -shared -o $@ $(LIB)

%.o: %.c
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f *.so
