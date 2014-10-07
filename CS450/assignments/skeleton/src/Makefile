GCC_OPTIONS=-Wall -pedantic -I../include -g
GL_OPTIONS=-lGLEW -lglut -lGL
COPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: prog

prog: initShader.o simpleProgram.o
	g++ -o prog initShader.o simpleProgram.o $(GL_OPTIONS)

initShader.o: initShader.cpp
	g++ $(GCC_OPTIONS) -c initShader.cpp

simpleProgram.o: simpleProgram.cpp
	g++ $(GCC_OPTIONS) -c simpleProgram.cpp

clean:
	rm initShader.o
	rm simpleProgram.o
	rm prog
