TEX     = write-up
CC      = gcc
LDFLAGS = -lpthread -lm -lrt
CFLAGS  = -std=c99 -Wall -g
TARGET  = sieve
INCLUDE = shared.h threaded.h main.h
OBJECTS = shared.o threaded.o
SOURCE  = shared.c threaded.c main.c

default: happy

.PHONY: help clean all default

all: pdf happy

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  all          to build writeup and myar executable"
	@echo "  uniqify      to build an executable"
	@echo "  pdf          to build the writeup"
	@echo "  clean        to remove unused files"

clean:
	-rm *.o *.out *.dvi *.log *.aux *.ps 2> /dev/null

pdf: ${TEX}.tex 
	latex $(TEX).tex
	dvips -R -Poutline -t letter ${TEX}.dvi -o ${TEX}.ps
	ps2pdf ${TEX}.ps

happy: $(OBJECTS) $(INCLUDES)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)

threaded.o: threaded.h
	$(CC) $(CFLAGS) -c threaded.c

shared.o: shared.h
	$(CC) $(CFLAGS) -c shared.c
