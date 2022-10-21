CFLAGS=-std=c99 -Wall -Wextra -g

llvm-rmt: main.o mappings.o matchings.o
	gcc $(CFLAGS) -o $@ $^

%.o : %.c
	gcc $(CFLAGS) -c $<

.PHONY = clean
clean :
	rm *.o llvm-rmt *~

