CFLAGS=-std=c99 -Wall -Wextra -g

bin-dir=bin
src-dir=src

bin/llvm-rmt: bin/main.o bin/mappings.o bin/matchings.o bin/llvm-ir-dispatch.o
	gcc $(CFLAGS) -o $@ $^ -lm

${bin-dir}/%.o : ${src-dir}/%.c
	gcc $(CFLAGS) -c $< -o $@

.PHONY = clean
clean :
	rm ${bin-dir}/* llvm-rmt

