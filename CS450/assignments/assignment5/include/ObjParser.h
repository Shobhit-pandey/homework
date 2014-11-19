#pragma once

#include <vector>
#include "Angel.h"

using Angel::vec4;
using std::vector;

// ObjParser
// Holds all the verticies, normals, and indexes listed in the obj file.
class ObjParser {
public:
    ObjParser(const char* objFilename);
    ~ObjParser();

    void print();
    void exportObj();

    vector<vec4> vertices;
    vector<vec4> normals;
    vector<GLuint> faces;

    const char* filename;

private:
    FILE* fp;
    unsigned int lineNumber;
    char lookahead[256];
};
