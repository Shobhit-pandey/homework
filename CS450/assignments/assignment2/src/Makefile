GCC_OPTIONS=-Wall -pedantic -I../include -g
GL_OPTIONS=-lGLEW -lglut -lGL
COPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: prog

prog: initShader.o assignment2.o
	g++ -o prog initShader.o assignment2.o $(GL_OPTIONS)

initShader.o: initShader.cpp
	g++ $(GCC_OPTIONS) -c initShader.cpp

assignment2.o: assignment2.cpp
	g++ $(GCC_OPTIONS) -c assignment2.cpp

clean:
	rm initShader.o
	rm assignment2.o
	rm prog
