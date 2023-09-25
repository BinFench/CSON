CC=gcc
CFLAGS= -c -Wall -Werror -fpic
DEBUG_FLAGS= -g
RELEASE_FLAGS= -O3
OBJS=CSON.o libCBoil.o
SHARED=libCSON.so
SHARED_FLAGS= -shared -o
INC=/usr/include
LIB=/usr/lib

default: install

install: $(OBJS)
	@ld -r -o libCSON.o $(OBJS)
	@$(CC) $(SHARED_FLAGS) $(SHARED) libCSON.o
	@sudo cp $(SHARED) $(LIB)
	@sudo cp CSON.h $(INC)
	@sudo cp CSON.def $(INC)
	@sudo chmod 0755 $(LIB)/$(SHARED)
	@sudo ldconfig
	@rm $(OBJS) $(SHARED) libCSON.o
	@echo "Shared library installed."

uninstall:
	@sudo rm $(INC)/CSON.h
	@sudo rm $(INC)/CSON.def
	@sudo rm $(LIB)/$(SHARED)
	@sudo ldconfig
	@echo "Shared library uninstalled."

%.o: %.c
	@$(CC) -c -o $@  $< $(CFLAGS) $(RELEASE_FLAGS) $(LIBS)