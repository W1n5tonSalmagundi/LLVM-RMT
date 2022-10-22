CFLAGS=-std=c99 -Wall -Wextra -g

bin-dir=bin
src-dir=src

bin/llvm-rmt: bin/main.o bin/mappings.o bin/matchings.o bin/llvm-ir-dispatch.o
	gcc $(CFLAGS) -o $@ $^ -lm

${bin-dir}/%.o : ${src-dir}/%.c ${bin-dir}
	gcc $(CFLAGS) -c $< -o $@

${bin-dir} :
	mkdir ${bin-dir}

.PHONY = clean
clean :
	rm ${bin-dir}/*

