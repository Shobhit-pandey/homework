#ifndef __BRAMWELT_SCENE_PARSER__
#define __BRAMWELT_SCENE_PARSER__

using Angel::Perspective;
using Angel::Ortho;
using Angel::mat4;
using Angel::vec3;

namespace scene {

// ParserState struct
// Holds all the verticies, normals, and indexes listed in the obj file.
struct SceneState
{
    vec3 eye;
    vec3 at;
    vec3 up;
    mat4 lens;

    //vec4 lens;

    FILE* fp;
    int line;
    char lookahead[256];
};

void printSceneState(SceneState* ss);
void line(SceneState* ss);
void parse(SceneState* ss, const char* filename);

}
#endif
