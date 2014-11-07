GCC_OPTIONS=-Wall -pedantic -I../include
GL_OPTIONS=-lGLEW -lglut -lGL
COPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: prog

prog: sceneParser.o objParser.o initShader.o assignment3.o
	g++ -o prog sceneParser.o objParser.o initShader.o assignment3.o $(GL_OPTIONS)

objParser.o: objParser.cpp
	g++ $(GCC_OPTIONS) -c objParser.cpp

sceneParser.o: sceneParser.cpp
	g++ $(GCC_OPTIONS) -c sceneParser.cpp

initShader.o: initShader.cpp
	g++ $(GCC_OPTIONS) -c initShader.cpp

assignment3.o: assignment3.cpp
	g++ $(GCC_OPTIONS) -c assignment3.cpp

clean:
	-rm initShader.o
	-rm objParser.o
	-rm sceneParser.o
	-rm assignment3.o
	-rm prog
