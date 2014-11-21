#pragma once

#include <vector>
#include "ObjParser.h"

using Angel::vec4;

class Mesh {
public:
    Mesh(const char* objFilename);
    Mesh(const char* objFilename, Angel::vec4 color);
    ~Mesh();

    static GLuint colorId;

    void draw(GLuint program);
    void wireframe(GLuint program);
    Angel::mat4 transform(); // Matrix of all transformations

    GLuint vao; // vertex array
    GLuint vbo; // vertex buffer
    GLuint ebo; // element buffer

    Angel::mat4 offset; // Base Translation offset.
    Angel::mat4 translate; // Translation matrix
    Angel::mat4 scale; // Scale matrix
    Angel::mat4 rotate; // Rotation matrix

    vector<vec4> colors;
    vec4 objColor;

private:
    ObjParser* ps;
    GLuint shader;
    void setupBuffers();
    void setupShaders(GLuint program);
    void setColor(Angel::vec4 color);
    void bindBuffers();
    void unbindBuffers();
    void genColors();
};
