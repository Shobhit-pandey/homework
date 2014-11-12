#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "objParser.h"

using std::vector;

ObjParser::ObjParser(const char* objFilename) {
    // Parse the file and set aside buffers
    parse(objFilename);
    setupBuffers();
}

ObjParser::~ObjParser() {
    // Delete buffers
	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers(1, &vbo);
	//glDeleteVertexArrays(1, &vao);
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
            GLuint xv, xn, yv, yn, zv, zn;
            sscanf(lookahead, "f %u//%u %u//%u %u//%u", &xv, &xn, &yv, &yn, &zv, &zn);
            faces.push_back(xv-1);
            faces.push_back(yv-1);
            faces.push_back(zv-1);
        }

        lineNumber++;
    }
}

void ObjParser::print() {
    printf("\tLines: %u\n", lineNumber);
    printf("\tVertices: %ld\n", vertices.size());
    printf("\tNormals: %ld\n", normals.size());
    printf("\tFaces: %ld\n", (faces.size()/3));
}

void ObjParser::exportObj() {
    for (unsigned int i = 0; i < vertices.size(); ++i) {
        printf("v %f %f %f\n", vertices[i][0], vertices[i][1], vertices[i][2]);
    }
    for (unsigned int i = 0; i < normals.size(); ++i) {
        printf("vn %f %f %f\n", normals[i][0], normals[i][1], normals[i][2]);
    }
    for (unsigned int i = 0; i < faces.size()/3; ++i) {
        printf("f %u %u %u\n", faces[i*3+0], faces[i*3+1], faces[i*3+2]);
    }
}

void ObjParser::setupBuffers() {
    // Generate buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // vao
    glBindVertexArray(vao);

    // Generate colors
    genColors();

    // Determine sizes for buffer offsets.
    GLsizeiptr verticesSize = sizeof(Angel::vec4) * vertices.size();
    GLsizeiptr normalsSize = sizeof(Angel::vec4) * normals.size();
    GLsizeiptr colorsSize = sizeof(Angel::vec4) * colors.size();
    GLsizeiptr facesSize = sizeof(GLuint) * faces.size();

    // vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize + normalsSize + colorsSize, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, colorsSize, colors.data());

    // ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesSize, faces.data(), GL_STATIC_DRAW);

    unbindBuffers();
}

Angel::mat4 ObjParser::transform() {
    return translate * scale * rotate;
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
    // Generate a random color for each vertex using colorId, assigning it to objColor.
    for (unsigned int i = 0; i < vertices.size(); ++i) {
        GLuint r = (colorId & 0x000000FF) >>  0;
        GLuint g = (colorId & 0x0000FF00) >>  8;
        GLuint b = (colorId & 0x00FF0000) >> 16;

        Angel::vec4 unique_color(r/255.0f, g/255.0f, b/255.0f, 1.0);
        colors.push_back(unique_color);
    }

    objColor = colorId;
    ++colorId;
}