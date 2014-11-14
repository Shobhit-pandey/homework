#pragma once

#include <vector>
#include "ObjParser.h"

using Angel::vec4;

class Mesh {
public:
    Mesh(const char* objFilename);
    ~Mesh();

    void draw();
    void wireframe();
    void setupShaders(GLuint program);
    void bindBuffers();
    void unbindBuffers();
    Angel::mat4 transform(); // Matrix of all transformations

    GLuint shader_program; // shader program (vertex & fragment)

    GLuint vao; // vertex array
    GLuint vbo; // vertex buffer
    GLuint ebo; // element buffer

    Angel::mat4 translate; // Translation matrix
    Angel::mat4 scale; // Scale matrix
    Angel::mat4 rotate; // Rotation matrix

    vector<vec4> colors;
    GLuint objColor;
    static GLuint colorId;

private:
    ObjParser* ps;
    void setupBuffers();
    void genColors();
};
