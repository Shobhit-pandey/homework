#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "objParser.h"

using std::vector;

ObjParser::ObjParser(const char* objFilename) {
    lineNumber = 0;
    filename = objFilename;
    fp = fopen(filename, "r");

    if (fp == 0) {
        fprintf(stderr, "Failed to open: %s\n", filename);
    }

    parse();
}

ObjParser::~ObjParser() {}

void ObjParser::parse(void) {
    int errno = 0;
    int numread;

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

void ObjParser::print(void) {
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

