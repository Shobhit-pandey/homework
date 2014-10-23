//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "Angel.h"
#include "objParser.h"
#include <stdio.h>

using Angel::vec4;
using Angel::mat4;
using Angel::Perspective;
using Angel::InitShader;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection; // projection matrix uniform shader variable location

std::vector<ParserState> objects;

int
vec_size(std::vector<vec4> v) {
    return sizeof(v[0])*v.size();
}

int
vec_size_int(std::vector<int> v) {
    return sizeof(v[0])*v.size();
}

mat4 p = Perspective (75.0, 1.0, 0.01, 15.0);

GLfloat eye_x = 0.0;
GLfloat eye_y = 0.0;

GLfloat pos_x = 0.0;
GLfloat pos_y = 0.0;
GLfloat pos_z = 1.0;

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
    point4 eye( pos_x, pos_y, pos_z, 1.0);
    point4  at( eye_x, eye_y, 0.0, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    mat4  mv = LookAt( eye, at, up );

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );

    for (unsigned int i = 0; i < objects.size(); ++i) {
        glDrawElements( GL_TRIANGLES, objects[i].indexes.size(), GL_UNSIGNED_INT, 0 );
    }
    glutSwapBuffers();
}

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033:  // Escape key
	case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    case '4':
        eye_x -= 0.10;
        break;
    case '6':
        eye_x += 0.10;
        break;
    case '2':
        eye_y -= 0.10;
        break;
    case '8':
        eye_y += 0.10;
        break;
    case '5':
        printf("Camera: \t%f %f %f\nLooking At:\t%f %f %f\n",
               pos_x, pos_y, pos_z,
               eye_x, eye_y, 0.0);
        break;
    case 'w':
        pos_z -= 0.10;
        break;
    case 's':
        pos_z += 0.10;
        break;
    case 'a':
        pos_x -= 0.10;
        eye_x -= 0.10;
        break;
    case 'd':
        pos_x += 0.10;
        eye_x += 0.10;
        break;
    case 'q':
        pos_y -= 0.10;
        eye_y -= 0.10;
        break;
    case 'e':
        pos_y +=0.10;
        eye_y += 0.10;
        break;
    }
    glutPostRedisplay();
}

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

void
readObjFilenames(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        ParserState ps;
        parse(&ps, argv[i]);
        objects.push_back(ps);
    }
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        printf("Usage: %s filename.obj [filename.obj, ...]\n\n", argv[0]);
        exit(0);
    }

    printf("\n"
           "The camera location can be controlled with W,A,S,D, and Q,E.\n"
           "The point to lookat can be controlled with 4,6,8,2, and 7,9.\n"
           "\n"
           "All changes to the camera are done in 0.1 increments.\n"
           "\n"
           "Eye:\n"
           "\tW\tMove the camera along +Z.\n"
           "\tS\tMove the camera along -Z.\n"
           "\tA\tMove the camera along +X.\n"
           "\tD\tMove the camera along -X.\n"
           "\tQ\tMove the camera along +Y.\n"
           "\tE\tMove the camera along -Y.\n"
           "At:\n"
           "\t4\tLook  left (-X).\n"
           "\t6\tLook right (+X).\n"
           "\t8\tLook    up (+Y).\n"
           "\t2\tLook  down (-Y).\n"
           "\t7\tLook    up (+Z).\n"
           "\t9\tLook  down (-Z).\n"
           "\n"
          );

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

    readObjFilenames(argc, argv);

    // Pass to init
    for (unsigned int i = 0; i < objects.size(); ++i) {
        init(&objects[i]);
    }

    //NOTE:  callbacks must go after window is created!!!
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();

    return(0);
}
