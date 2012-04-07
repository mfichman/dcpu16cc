CFLAGS = -O0 -g -Werror -Wall -pedantic

dcpu16cc: lexer.o parser.o main.o env.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o dcpu16cc
