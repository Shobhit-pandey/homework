GCC_OPTIONS=-Wall -pedantic -I../include -g
GL_OPTIONS=-lGLEW -lglut -lGL
COPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: prog

prog: initShader.o assignment1.o
	g++ -o prog initShader.o assignment1.o $(GL_OPTIONS)

initShader.o: initShader.cpp
	g++ $(GCC_OPTIONS) -c initShader.cpp

assignment1.o: assignment1.cpp
	g++ $(GCC_OPTIONS) -c assignment1.cpp

clean:
	rm initShader.o
	rm assignment1.o
	rm prog
