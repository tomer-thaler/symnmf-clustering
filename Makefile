CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors -lm

symnmf: symnmf.h symnmf.o
	gcc -o symnmf symnmf.o $(CFLAGS)

symnmf.o: symnmf.c
	gcc -c symnmf.c $(CFLAGS)

