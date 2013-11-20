TEX     = write-up
CC      = gcc
LDFLAGS = -lpthread
CFLAGS  = -std=c99 -Wall

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

happy: threaded.o shared.o main.c main.h
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o bin/happy

threaded.o: threaded.c threaded.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c threaded.c

shared.o: shared.c shared.h
	$(CC) $(CFLAGS) -c shared.c
