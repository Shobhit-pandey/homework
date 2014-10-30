//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "Angel.h"
#include "objParser.h"
#include "sceneParser.h"
#include <stdio.h>

using scene::SceneState;
using obj::ParserState;

using Angel::vec4;
using Angel::mat4;
using Angel::Perspective;
using Angel::Ortho;
using Angel::InitShader;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection; // projection matrix uniform shader variable location

// SceneState hold viewing parameters
SceneState ss;

// ParserState hold parsed objects
std::vector<ParserState> objects;
// There is one vao for each object file that is loaded
std::vector<GLuint> vaos;

// Helper function for getting the size in bytes of a Angel::vec4
int
vec_size(std::vector<vec4> v) {
    return sizeof(v[0])*v.size();
}

int
vec_size_int(std::vector<int> v) {
    return sizeof(v[0])*v.size();
}

// Usage information
char help_msg[] = "\n"
   "The camera location can be controlled with W,A,S,D, and Q,E.\n"
   "The point to lookat can be controlled with 4,6,8,2, and 7,9.\n"
   "\n"
   "All changes to the camera are done in 0.1 increments.\n"
   "\n"
   "Eye:\n"
   "\tA\tMove    right (+X).\n"
   "\tD\tMove     left (-X).\n"
   "\tQ\tMove       up (+Y).\n"
   "\tE\tMove     down (-Y).\n"
   "\tW\tMove  forward (+Z).\n"
   "\tS\tMove backward (-Z).\n"
   "At:\n"
   "\t6\tLook   right (+X).\n"
   "\t4\tLook    left (-X).\n"
   "\t8\tLook      up (+Y).\n"
   "\t2\tLook    down (-Y).\n"
   "\t9\tLook further (+Z).\n"
   "\t7\tLook  nearer (-Z).\n"
   "Up:\n"
   "\tJ\tYaw    left (+X)\n"
   "\tL\tYaw   right (-X)\n"
   "\tI\tPitch    up (+Y)\n"
   "\tK\tPitch  down (-Y)\n"
   "\tU\tRoll   left (+Z)\n"
   "\tO\tRoll  right (-Z)\n"
   "\n";

// OpenGL initialization
void
init(ParserState* ps)
{

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    vaos.push_back(vao);

    // Create and initialize a buffer object
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER,
                  vec_size(ps->vertices) + vec_size(ps->normals),
                  NULL,
                  GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER,
                     0,
                     vec_size(ps->vertices),
                     &ps->vertices[0] );
    glBufferSubData( GL_ARRAY_BUFFER,
                     vec_size(ps->vertices),
                     vec_size(ps->normals),
                     &ps->normals[0] );

    // Create an Element Array Buffer
    GLuint ebo;
    glGenBuffers( 1, &ebo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                  ps->indexes.size()*sizeof(&ps->indexes[0]),
                  &ps->indexes[0],
                  GL_STATIC_DRAW );

    // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           0,
                           BUFFER_OFFSET( (long int)vec_size(ps->vertices) ) );

    // Initialize shader lighting parameters
    // RAM: No need to change these...we'll learn about the details when we
    // cover Illumination and Shading
    point4 light_position( 1.5, 1.5, 2.0, 1.0 );
    color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    color4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    color4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    color4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product );

    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
		  1, light_position );

    glUniform1f( glGetUniformLocation(program, "Shininess"),
		 material_shininess );


    model_view = glGetUniformLocation( program, "ModelView" );
    projection = glGetUniformLocation( program, "Projection" );


    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Update Camera
    point4 eye( ss.eye, 1.0 );
    point4  at( ss.at, 1.0 );
    vec4    up( ss.up, 0.0 );

    mat4  mv = LookAt( eye, at, up );

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );
    glUniformMatrix4fv( projection, 1, GL_TRUE, ss.lens );

    // Render each loaded object file.
    for (unsigned int i = 0; i < objects.size(); ++i) {
        glBindVertexArray(vaos[i]);
        glDrawElements( GL_TRIANGLES, objects[i].indexes.size(), GL_UNSIGNED_INT, 0 );
    }
    glutSwapBuffers();
}

// Keys primarily control the camera parameters
void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033:  // Escape key
	case 'Q': exit( EXIT_SUCCESS ); break;
    case '4': ss.at[0] -= 0.10; break;
    case '6': ss.at[0] += 0.10; break;
    case '2': ss.at[1] -= 0.10; break;
    case '8': ss.at[1] += 0.10; break;
    case '7': ss.at[2] += 0.10; break;
    case '9': ss.at[2] -= 0.10; break;
    case '5':
        printf("eye \t%.2f %.2f %.2f\n"
               "at \t%.2f %.2f %.2f\n"
               "up \t%.2f %.2f %.2f\n",
               ss.eye[0], ss.eye[1], ss.eye[2],
               ss.at[0], ss.at[1], ss.at[2],
               ss.up[0], ss.up[1], ss.up[2]);
        break;
    case 'w': ss.eye[2] -= 0.10; break;
    case 's': ss.eye[2] += 0.10; break;
    case 'a': ss.eye[0] -= 0.10; break;
    case 'd': ss.eye[0] += 0.10; break;
    case 'q': ss.eye[1] -= 0.10; break;
    case 'e': ss.eye[1] += 0.10; break;
    case 'i': ss.up[0]  += 0.10; break;
    case 'k': ss.up[0]  -= 0.10; break;
    case 'j': ss.up[1]  += 0.10; break;
    case 'l': ss.up[1]  -= 0.10; break;
    case 'o': ss.up[2]  += 0.10; break;
    case 'u': ss.up[2]  -= 0.10; break;
    }
    glutPostRedisplay();
}

// Debug helper for printing the passed arguments.
void
printArgs(int argc, char** argv) {
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (i != argc && i != 1) printf(", ");
            printf("arg%d: %s", i, argv[i]);
        }
        printf("\n");
    }
}

// Parses each obj file passed in and adds it the the objects array.
void
readObjFilenames(int argc, char** argv) {
    for (int i = 2; i < argc; ++i) {
        ParserState ps;
        obj::parse(&ps, argv[i]);
        objects.push_back(ps);
    }
}

// Ortho(left, right, bottom, top, near, far)
// frustum(left, right, bottom, top, near, far)
void
myReshape(int w, int h) {
    glViewport(0, 0, w, h);
    float ar = w/h;
    if (ar < startAR) { // (w <= h) // taller
        proj = Ortho(
                vl,
                vr,
                vl * (GLfloat) h / (GLfloat) w,
                vr * (GLfloat) h / (GLfloat) w,
                0.1,
                10.0);
    } else { // wider
        proj = Ortho(
                vb * (GLfloat) w / (GLfloat) h,
                vt * (GLfloat) w / (GLfloa) h,
                vb,
                vt,
                0.1,
                10.0);
    }
}

// Parses the scene file and assigns it to 'ss'
void
readSceneFilename(char** argv) {
    scene::parse(&ss, argv[1]);
}

int main(int argc, char** argv)
{
    // If no arguments passed, print usage and exit
    if (argc == 1) {
        printf("Usage: %s scenefile.glsf filename.obj [filename.obj, ...]\n\n", argv[0]);
        exit(0);
    }

    // Print the help message each time the program is run.
    //   This provides users with a list of keyboard commands to use.
    printf("%s", help_msg);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion (3, 2);
    glutInitContextFlags (GLUT_FORWARD_COMPATIBLE);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(500, 300);
    glutCreateWindow("Simple Open GL Program");
    printf("%s\n%s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

    glewExperimental = GL_TRUE;
    glewInit();

    // Read and parse the scene file
    readSceneFilename(argv);
    // Read and parse each object file passed in
    readObjFilenames(argc, argv);

    // Pass objects to init
    for (unsigned int i = 0; i < objects.size(); ++i) {
        init(&objects[i]);
    }
    // Print the number of objects 'init-ed'
    printf("Initialized: %ld objects\n\n", objects.size());

    //NOTE:  callbacks must go after window is created!!!
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    //glutReshapeFunc(myReshape);
    glutMainLoop();

    return(0);
}
