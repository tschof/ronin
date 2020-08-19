all: libbaz.a baz test.so

libbaz.a: libbaz.c
	gcc -Wall -g -c -fPIC libbaz.c
	ar -cvq libbaz.a libbaz.o

baz: baz.c
	gcc -g -o baz -ldl baz.c

test.so: test.c
	gcc -g -shared -fPIC -o test.so test.c libbaz.a

clean:
	rm -f libbaz.[ao]
	rm -f test.so
	rm -f baz

