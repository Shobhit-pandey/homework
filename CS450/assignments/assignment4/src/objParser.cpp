#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "objParser.h"

using std::vector;

ObjParser::ObjParser(const char* objFilename) {
    // Parse the file and set aside buffers
    parse(objFilename);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    setupBuffers();
}

ObjParser::~ObjParser() {
    // Delete buffers
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void ObjParser::parse(const char* objFilename) {
    lineNumber = 0;
    filename = objFilename;
    fp = fopen(filename, "r");

    int errno = 0;
    int numread;

    if (fp == 0) {
        fprintf(stderr, "Failed to open: %s\n", filename);
    }

    while ((numread = fscanf(fp, "%[^\n]\n", lookahead)) != EOF) {
        if (errno != 0) {
            perror("fscanf");
        }

        if (numread <= 0) {
            fprintf(stderr, "Error reading file. Wrong number of chars.");
        }

        if (strncmp(lookahead, "#", 1) == 0) {
            lineNumber++;
            continue;
        }

        if (strncmp(lookahead, "vn", 2) == 0) {
            GLfloat x, y, z;
            sscanf(lookahead, "vn %f %f %f", &x, &y, &z);
            vec4 normal(x, y, z, 0.0);
            normals.push_back(normal);
        } else if (strncmp(lookahead, "v", 1) == 0) {
            GLfloat x, y, z;
            sscanf(lookahead, "v %f %f %f", &x, &y, &z);
            vec4 vertex(x, y, z, 1.0);
            vertices.push_back(vertex);
        } else if (strncmp(lookahead, "f", 1) == 0) {
            int xv, xn, yv, yn, zv, zn;
            sscanf(lookahead, "f %d//%d %d//%d %d//%d", &xv, &xn, &yv, &yn, &zv, &zn);
            faces.push_back(xv-1);
            faces.push_back(yv-1);
            faces.push_back(zv-1);
        }

        lineNumber++;
    }
}

void ObjParser::print() {
    printf("Vertices: %ld\n", vertices.size());
    printf("Normals: %ld\n", normals.size());
    printf("Faces: %ld\n", (faces.size()/3));
    printf("lines: %u\n", lineNumber);

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        printf("v %f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
    }
    for (unsigned int i = 0; i < normals.size(); ++i) {
        printf("vn %f %f %f\n", normals[i][0], normals[i][1], normals[i][2]);
    }
    for (unsigned int i = 0; i < faces.size()/3; ++i) {
        printf("f %d %d %d\n", faces[i*3+0], faces[i*3+1], faces[i*3+2]);
    }
}

void ObjParser::setupBuffers() {
    bindBuffers();

    // Generate colors
    genColors();

    GLsizeiptr verticesSize = sizeof(Angel::vec4) * vertices.size();
    GLsizeiptr normalsSize = sizeof(Angel::vec4) * normals.size();
    GLsizeiptr colorsSize = sizeof(Angel::vec4) * colors.size();
    GLsizeiptr facesSize = sizeof(unsigned int) * faces.size();

    // vao & vbo
    glBufferData( GL_ARRAY_BUFFER, verticesSize + normalsSize + colorsSize, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, verticesSize, vertices.data());
    glBufferSubData( GL_ARRAY_BUFFER, verticesSize, normalsSize, normals.data());
    glBufferSubData( GL_ARRAY_BUFFER, verticesSize + normalsSize, colorsSize, colors.data());

    // ebo
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, facesSize, faces.data(), GL_STATIC_DRAW );

    unbindBuffers();
}


void ObjParser::bindBuffers() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void ObjParser::unbindBuffers() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void ObjParser::genColors() {
    // Generate a random color for each vertex
    for (unsigned int i = 0; i < faces.size(); ++i) {
        GLuint r = (vao & 0x000000FF) >>  0;
        GLuint g = (vao & 0x0000FF00) >>  8;
        GLuint b = (vao & 0x00FF0000) >> 16;

        Angel::vec4 unique_color(r/255.0f, g/255.0f, b/255.0f, 1.0);
        colors.push_back(unique_color);
    }
}
