#pragma once

#include <vector>

using Angel::vec4;

// ObjParser
// Holds all the verticies, normals, and indexes listed in the obj file.
class ObjParser {
public:
    ObjParser(const char* objFilename);
    ~ObjParser();

    void print();
    void parse(const char* objFilename);
    void setupBuffers();
    void genColors();

    void bindBuffers();
    void unbindBuffers();

    std::vector<Angel::vec4> vertices;
    std::vector<Angel::vec4> normals;
    std::vector<unsigned int> faces;
    std::vector<Angel::vec4> colors;

    GLuint vao; // vertex array
    GLuint vbo; // vertex buffer
    GLuint ebo; // element buffer

    const char* filename;

private:
    FILE* fp;
    unsigned int lineNumber;
    char lookahead[256];
};
