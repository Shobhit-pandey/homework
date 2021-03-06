//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

#define WINDOW_WIDTH  512
#define WINDOW_HEIGHT 512

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
using Angel::DegreesToRadians;
using Angel::Ortho;
using Angel::Frustum;
using Angel::InitShader;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection; // projection matrix uniform shader variable location

GLuint swap_colors;
GLuint wireframe_vao = -1;

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
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    vaos.push_back(vao);

    // Generate a random color for each object
    std::vector<vec4> colors;
    for (unsigned int i = 0; i < ps->indexes.size(); ++i) {
    GLuint r = (vao & 0x000000FF) >>  0;
    GLuint g = (vao & 0x0000FF00) >>  8;
    GLuint b = (vao & 0x00FF0000) >> 16;
    color4 unique_color(r/255.0f, g/255.0f, b/255.0f, 1.0);
    colors.push_back(unique_color);
    }

    // Create and initialize a buffer object
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER,
                  vec_size(ps->vertices) + vec_size(ps->normals) + vec_size(colors),
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
    glBufferSubData( GL_ARRAY_BUFFER,
                     vec_size(ps->vertices)+vec_size(ps->normals),
                     vec_size(colors),
                     &colors[0]);

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

    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           0,
                           BUFFER_OFFSET( (long int)vec_size(ps->vertices)+vec_size(colors) ) );

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

    swap_colors = glGetUniformLocation( program, "Swap" );

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
    glUniformMatrix4fv( projection, 1, GL_TRUE, ss.proj );

    // Render each loaded object file.
    for (unsigned int i = 0; i < objects.size(); ++i) {
        glBindVertexArray(vaos[i]);
        if (wireframe_vao > 0 && wireframe_vao == vaos[i]) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glPolygonOffset( 1.0, 2.0 );
            glDrawElements( GL_TRIANGLES, objects[i].indexes.size(), GL_UNSIGNED_INT, 0 );
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            glDrawElements( GL_TRIANGLES, objects[i].indexes.size(), GL_UNSIGNED_INT, 0 );
        }
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
    case '5': printSceneState(&ss); break;
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


/**
 * Each object in the scene is assigned a unique color determined by
 *   it's index in the vao array.
 * On click:
 *   - Render scene in back buffer with color shader
 *   - Use object color to obtain index to vao array
 *   - Render scene in front buffer, but use wireframe for clicked
 *     object.
 */
void
mouse( int button, int state, int x, int y ) {
    //state is GLUT_UP or GLUT_DOWN
    //button is GLUT_{LEFT, RIGHT, MIDDLE}_BUTTON
    if (state == GLUT_UP) {

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glUniform1i( swap_colors, 1 );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Render each loaded object file.
        for (unsigned int i = 0; i < objects.size(); ++i) {
            glBindVertexArray(vaos[i]);
            glDrawElements( GL_TRIANGLES, objects[i].indexes.size(), GL_UNSIGNED_INT, 0 );
        }

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLubyte pixel[4];
        glReadPixels(x, viewport[3]-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

        // id corresponds to a specific vertex. That vertex is part of an
        // object.
        GLuint id =
            pixel[0] +
            pixel[1] * 256 +
            pixel[2] * 256*256;

        // Set wireframe_vao to vao index of clicked object.
        if (id > 0 && id <= vaos.size()) wireframe_vao = id;

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glUniform1i( swap_colors, 0 );
        // Don't swap buffers, just render over what's there.
        // Swapping buffers produces a flicker.
        glutPostRedisplay();
    }
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

void
myOrthoReshape(int w, int h) {
    glViewport(0, 0, w, h);

    GLfloat aspect = (GLfloat) w / (GLfloat) h;
    GLfloat left = ss.lens[0];
    GLfloat right = ss.lens[1];
    GLfloat bottom = ss.lens[2];
    GLfloat top = ss.lens[3];
    GLfloat zNear = ss.lens[4];
    GLfloat zFar = ss.lens[5];

    if (w > h) {
        ss.proj = Ortho(left*aspect, right*aspect, bottom, top, zNear, zFar);
    } else {
        ss.proj = Ortho(left, right, bottom/aspect, top/aspect, zNear, zFar);
    }
}

void
myPerspectiveReshape(int w, int h) {
    glViewport(0, 0, w, h);

    GLfloat fovy = ss.lens[0];
    GLfloat aspect = (GLfloat) w / (GLfloat) h;
    GLfloat zNear = ss.lens[2];
    GLfloat zFar = ss.lens[3];

    GLfloat top   = tan(fovy*DegreesToRadians/2) * zNear;
    GLfloat right = top*ss.lens[1];
    GLfloat left = -right;
    GLfloat bottom = -top;

    if ( w > h ) {
        ss.proj = Frustum(left*aspect, right*aspect, bottom, top, zNear, zFar);
    } else {
        ss.proj = Frustum(left, right, bottom/aspect, top/aspect, zNear, zFar);
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
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    if (ss.lens.size() == 4) {
        glutReshapeFunc(myPerspectiveReshape);
    } else {
        glutReshapeFunc(myOrthoReshape);
    }
    glutMainLoop();

    return(0);
}
