#ifndef __BRAMWELT_OBJ_PARSER__
#define __BRAMWELT_OBJ_PARSER__

#include <vector>

// ParserState struct
// Holds all the verticies, normals, and indexes listed in the obj file.
struct ParserState
{
    std::vector<vec4> vertices;
    std::vector<vec4> normals;
    std::vector<int> indexes;

    FILE* fp;
    int line;
    char lookahead[256];
};

void line(ParserState* ps);
void parse(ParserState* ps, const char* filename);
#endif
