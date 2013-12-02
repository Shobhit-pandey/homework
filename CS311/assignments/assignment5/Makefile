TEX     = write-up
CC      = gcc
LDFLAGS = -lpthread -lm -lrt
CFLAGS  = -std=c99 -Wall -g
TARGET  = compute
INCLUDE = compute.h
SOURCE  = compute.c

default: compute

.PHONY: help clean all default

all: pdf compute

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

compute: $(SOURCE) $(INCLUDE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET) $(LDFLAGS)
