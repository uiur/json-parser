objects = parse.o value.o

main: main.c $(objects)
test: test.c $(objects)

debug: CFLAGS = -g
debug: main

parse.o: parse.c parse.h
value.o: value.c value.h

.PHONY: clean
clean:
	rm -rf ./main $(objects)
