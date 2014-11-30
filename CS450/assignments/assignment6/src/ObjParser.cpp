#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "ObjParser.h"

using std::vector;
using Angel::vec4;

// Parse the file and set aside buffers
ObjParser::ObjParser(const char* objFilename) {
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

ObjParser::~ObjParser() {}

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
