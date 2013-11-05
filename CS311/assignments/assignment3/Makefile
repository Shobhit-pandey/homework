TEX     = write-up
SRC     = uniqify
CC      = gcc
CFLAGS  = -std=c99 -Wall

default: help

.PHONY: help clean all default

all: pdf myar

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

uniqify: 
	$(CC) $(CFLAGS) $(SRC).c -o $(SRC).o
