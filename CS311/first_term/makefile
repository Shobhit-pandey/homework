# Makefile defines
CC = gcc
CFLAGS = -Wall -std=c99
SRC = main.c 
OBJECTS = $(addsuffix .o, $(basename $(SRC)))
PROG = prog

# Makefile commands
default: $(OBJECTS)
	gcc -Wall -ansi -o $(PROG) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJECTS)

cleanall: clean
	rm $(PROG)
	
