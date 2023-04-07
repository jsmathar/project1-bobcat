CC=clang
CFLAGS=-Wall -Wextra -std=c11 -pedantic

all: bobcat

bobcat: bobcat.c

debug: bobcat.c
	$(CC) $(CFLAGS) -g $< -o bobcat

format:
	clang-format -i bobcat.c

check-format:
	clang-format --dry-run --Werror bobcat.c

test: bobcat
	./test/bats/bin/bats test/*.bats

clean:
	rm bobcat
