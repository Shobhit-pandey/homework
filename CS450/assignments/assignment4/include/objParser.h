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
    void exportObj();
    void parse(const char* objFilename);

    void bindBuffers();
    void unbindBuffers();

    std::vector<Angel::vec4> vertices;
    std::vector<Angel::vec4> normals;
    std::vector<GLuint> faces;
    std::vector<Angel::vec4> colors;

    GLuint vao; // vertex array
    GLuint vbo; // vertex buffer
    GLuint ebo; // element buffer

    const char* filename;

    GLuint objColor;
    static GLuint colorId;

private:
    void genColors();
    void setupBuffers();

    FILE* fp;
    unsigned int lineNumber;
    char lookahead[256];
};
