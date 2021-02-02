objects = parse.o

main: main.c $(objects)
test: test.c $(objects)
parse.o: parse.c parse.h

.PHONY: clean
clean:
	rm -rf ./main $(objects)
