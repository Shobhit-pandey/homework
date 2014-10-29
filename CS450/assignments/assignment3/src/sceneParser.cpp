#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "sceneParser.h"

namespace scene {


void
printSceneState(SceneState* ss) {
    printf("Up: %f %f %f\n", ss->up[0], ss->up[1], ss->up[2]);
    printf("Eye: %f %f %f\n", ss->eye[0], ss->eye[1], ss->eye[2]);
    printf("At: %f %f %f\n", ss->at[0], ss->at[1], ss->at[2]);
    for(int i=0; i<4; i++) printf("Lens: %f %f %f %f\n", ss->lens[i][0], ss->lens[i][1], ss->lens[i][2], ss->lens[i][3]);
}

void
line(SceneState* ss)
{
    int errno = 0;
    int numread;

    while ((numread = fscanf(ss->fp, "%[^\n]\n", ss->lookahead)) != EOF) {
        if (errno != 0) {
            perror("fscanf");
        }

        if (numread <= 0) {
            fprintf(stderr, "Error reading file. Wrong number of chars.");
        }

        if (strncmp(ss->lookahead, "#", 1) == 0) {
            ss->line++;
            continue;
        }

        if (strncmp(ss->lookahead, "up", 2) == 0) {
            GLfloat x, y, z;
            sscanf(ss->lookahead, "up %f %f %f", &x, &y, &z);
            vec3 up_vec(x, y, z);
            ss->up = up_vec;
        } else if (strncmp(ss->lookahead, "at", 2) == 0) {
            GLfloat x, y, z;
            sscanf(ss->lookahead, "at %f %f %f", &x, &y, &z);
            vec3 at_vec(x, y, z);
            ss->at = at_vec;
        } else if (strncmp(ss->lookahead, "p", 1) == 0) {
            GLfloat fovy, aspect, near, far;
            sscanf(ss->lookahead, "p %f %f %f %f", &fovy, &aspect, &near, &far);
            mat4 pers_mat = Perspective(fovy, aspect, near, far);
            ss->lens = pers_mat;
        } else if (strncmp(ss->lookahead, "o", 1) == 0) {
            GLfloat left, right, bottom, top, near, far;
            sscanf(ss->lookahead, "o %f %f %f %f %f %f", &left, &right, &bottom, &top, &near, &far);
            mat4 ortho_mat = Ortho(left, right, bottom, top, near, far);
            ss->lens = ortho_mat;
        } else if (strncmp(ss->lookahead, "eye", 3) == 0) {
            GLfloat x, y, z;
            sscanf(ss->lookahead, "eye %f %f %f", &x, &y, &z);
            vec3 eye_vec(x, y, z);
            ss->eye = eye_vec;
        }

        ss->line++;
    }
}

void
parse(SceneState* ss, const char* filename)
{
    ss->line = 0;
    ss->fp = fopen(filename, "r");

    if (ss->fp == 0) {
        printf("Failed to create SceneState or open: %s", filename);
    }

    line(ss);
}

}
