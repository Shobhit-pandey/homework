#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "objParser.h"

using std::vector;


void
line(ParserState* ps)
{
    int errno = 0;
    int numread;

    while ((numread = fscanf(ps->fp, "%[^\n]\n", ps->lookahead)) != EOF) {
        if (errno != 0) {
            perror("fscanf");
        }

        if (numread <= 0) {
            fprintf(stderr, "Error reading file. Wrong number of chars.");
        }

        if (strncmp(ps->lookahead, "#", 1) == 0) {
            ps->line++;
            continue;
        }

        if (strncmp(ps->lookahead, "vn", 2) == 0) {
            GLfloat x, y, z;
            sscanf(ps->lookahead, "vn %f %f %f", &x, &y, &z);
            vec4 normal(x, y, z, 0.0);
            ps->normals.push_back(normal);
        } else if (strncmp(ps->lookahead, "v", 1) == 0) {
            GLfloat x, y, z;
            sscanf(ps->lookahead, "v %f %f %f", &x, &y, &z);
            vec4 vertex(x, y, z, 1.0);
            ps->vertices.push_back(vertex);
        } else if (strncmp(ps->lookahead, "f", 1) == 0) {
            int xv, xn, yv, yn, zv, zn;
            sscanf(ps->lookahead, "f %d//%d %d//%d %d//%d", &xv, &xn, &yv, &yn, &zv, &zn);
            ps->indexes.push_back(xv-1);
            ps->indexes.push_back(yv-1);
            ps->indexes.push_back(zv-1);
        }

        ps->line++;
    }
}

void
parse(ParserState* ps, const char* filename)
{
    ps->line = 0;
    ps->fp = fopen(filename, "r");

    if (ps->fp == 0) {
        printf("Failed to create ParserState or open: %s", filename);
    }

    line(ps);
}
