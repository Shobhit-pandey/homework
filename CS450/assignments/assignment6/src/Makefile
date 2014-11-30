GCC_OPTIONS=-Wall -pedantic -I../include
GL_OPTIONS=-lGLEW -lglut -lGL
COPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)

all: prog

prog: sceneParser.o ObjParser.o Mesh.o initShader.o assignment6.o
	g++ -o prog sceneParser.o ObjParser.o Mesh.o initShader.o assignment6.o $(GL_OPTIONS)

ObjParser.o: ObjParser.cpp
	g++ $(GCC_OPTIONS) -c ObjParser.cpp

sceneParser.o: sceneParser.cpp
	g++ $(GCC_OPTIONS) -c sceneParser.cpp

Mesh.o: Mesh.cpp ObjParser.cpp
	g++ $(GCC_OPTIONS) -c Mesh.cpp

initShader.o: initShader.cpp
	g++ $(GCC_OPTIONS) -c initShader.cpp

assignment6.o: assignment6.cpp
	g++ $(GCC_OPTIONS) -c assignment6.cpp

clean:
	-rm initShader.o
	-rm ObjParser.o
	-rm sceneParser.o
	-rm Mesh.o
	-rm assignment6.o
	-rm prog
