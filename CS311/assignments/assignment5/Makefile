TEX     = write-up
CC      = icc
LDFLAGS = -lpthread -lm -lrt
CFLAGS  = -std=c99 -Wall -g
TARGET  = compute
INCLUDE = compute.h main.h
SOURCE  = compute.c main.c

default: main

.PHONY: help clean all default

all: pdf main

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  all          to build writeup and myar executable"
	@echo "  compute      to build an executable"
	@echo "  pdf          to build the writeup"
	@echo "  clean        to remove unused files"

clean:
	-rm *.o *.out *.dvi *.log *.aux *.ps 2> /dev/null

pdf: ${TEX}.tex 
	latex $(TEX).tex
	dvips -R -Poutline -t letter ${TEX}.dvi -o ${TEX}.ps
	ps2pdf ${TEX}.ps

main: $(SOURCE) $(INCLUDE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
