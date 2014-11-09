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
    void parse();

    std::vector<Angel::vec4> vertices;
    std::vector<Angel::vec4> normals;
    std::vector<unsigned int> faces;

    const char* filename;

private:
    FILE* fp;
    unsigned int lineNumber;
    char lookahead[256];
};
