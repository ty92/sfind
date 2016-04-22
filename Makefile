CC=gcc
sfind: sfind_1.c
	${CC} sfind_1.c -o sfind
clean:
	rm -fr *.o
	rm -fr sfind
