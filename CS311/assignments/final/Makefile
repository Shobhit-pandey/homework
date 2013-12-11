TEX     = write-up
CFLAGS  = -Wall -std=c99 -g
CC		= gcc

default: pdf

.PHONY: help clean all default

all: pdf progs

help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  all          to build writeup and myar executable"
	@echo "  pdf          to build the writeup"
	@echo "  clean        to remove unused files"

clean:
	-rm *.o *.out *.dvi *.log *.aux *.ps 2> /dev/null

cleanall: clean
	-rm posix_client posix_server posix_pipes posix_procs 2> /dev/null

pdf: ${TEX}.tex 
	latex $(TEX).tex
	dvips -R -Poutline -t letter ${TEX}.dvi -o ${TEX}.ps
	ps2pdf ${TEX}.ps

progs: sockets pipes procs

sockets:
	$(CC) $(CFLAGS) posix_sockets_client.c -o posix_client
	$(CC) $(CFLAGS) posix_sockets_server.c -o posix_server

pipes:
	$(CC) $(CFLAGS) posix_pipes.c -o posix_pipes

procs:
	$(CC) $(CFLAGS) posix_procs.c -o posix_procs

