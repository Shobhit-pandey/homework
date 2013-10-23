TARGET  = write-up
SRC     = myar
CC      = gcc
TESTS   = test-ar.py
LDFLAGS =
CFLAGS  = -std=c99 -Wall

.PHONY: help clean all

#default: help
default: myar
#default: pdf

all: pdf myar

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  all          to build writeup and myar executable"
	@echo "  test         run tests on current build of myar"
	@echo "  myar         to build a myar executable"
	@echo "  pdf          to build the writeup"
	@echo "  clean        to remove unused files"

clean:
	-rm *.o *.out *.dvi *.log *.aux *.ps 2> /dev/null

pdf: ${TARGET}.tex 
#	pygmentize the input source file -- THIS NAME SHOULD BE SAFE
#	pygmentize -f latex -o __${TARGET}.tex ${TARGET}.tex
#	run latex twice to get references correct
	latex ${TARGET}.tex
#	you can also have a bibtex line here
#	bibtex $(TARGET)
	latex $(TARGET).tex
#	remove the pygmentized output to avoid cluttering up the directory
#	rm __${TARGET}.tex 
	dvips -R -Poutline -t letter ${TARGET}.dvi -o ${TARGET}.ps
	ps2pdf ${TARGET}.ps

myar: 
	$(CC) $(CFLAGS) $(SRC).c $(LDFLAGS) -o $(SRC).o

test:
	python $(TESTS)
