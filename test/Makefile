CC=gcc
CFLAGS= -Wall -Werror
DEBUG_FLAGS= -g
RELEASE_FLAGS= -O3
LIBS= -lCSON

default:
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o test test_parse.c $(LIBS)
	@sudo chmod 0755 test
	@./test
	@rm test

test-mem:
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o test test_parse.c $(LIBS)
	@sudo chmod 0755 test
	@valgrind -v --track-origins=yes --leak-check=full --show-leak-kinds=all ./test
	@rm test

clean:
	@rm -f test