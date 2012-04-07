CFLAGS = -O0 -g -Werror -Wall -pedantic

dcpu16cc: lexer.o main.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o dcpu16cc
