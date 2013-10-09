# Makefile defines
CC = gcc
CFLAGS = -g -Wall -pedantic -std=c99
SRC = myar.c myar_lib.c
INCLUDES = myar_lib.h
OBJECTS = $(addsuffix .o, $(basename $(SRC)))
PROG = myar

define cc-command
$(CC) $(CFLAGS) -o $(PROG) $(OBJECTS)
endef

# Makefile commands
default: $(OBJECTS) $(INCLUDES)
	$(cc-command)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJECTS)

cleanall: clean
	rm $(PROG)
	
