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

GLuint ObjParser::colorId = 0;

using scene::SceneState;

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
GLint   transformation;

GLuint swap_colors;
GLuint wireframe_vao = -1;

// SceneState hold viewing parameters
SceneState ss;

// ObjParser hold parsed objects
std::vector<ObjParser> objects;

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
init(ObjParser *ps)
{
    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    ps->bindBuffers();

    // vPosition
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // vNormal
    GLint64 verticesSize = sizeof(Angel::vec4) * ps->vertices.size();
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           0,
                           BUFFER_OFFSET(verticesSize) );

    // vColor
    GLint64 normalsSize = sizeof(Angel::vec4) * ps->normals.size();
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           0,
                           BUFFER_OFFSET((verticesSize + normalsSize)) );

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
    transformation = glGetUniformLocation( program, "Transform" );

    swap_colors = glGetUniformLocation( program, "Swap" );

    ps->unbindBuffers();
}

void
display( void )
{
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glUniform1i( swap_colors, 0 );

    // Update Camera
    point4 eye( ss.eye, 1.0 );
    point4  at( ss.at, 1.0 );
    vec4    up( ss.up, 0.0 );

    mat4  mv = LookAt( eye, at, up );

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );
    glUniformMatrix4fv( projection, 1, GL_TRUE, ss.proj );

    // Render each loaded object file.
    for (unsigned int i = 0; i < objects.size(); ++i) {
        objects[i].bindBuffers();
        // Apply transformations
        glUniformMatrix4fv(transformation, 1, GL_TRUE, objects[i].transform);
        if (wireframe_vao == i) {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glPolygonOffset( 1.0, 2.0 );
        } else {
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        glDrawElements( GL_TRIANGLES, objects[i].faces.size(), GL_UNSIGNED_INT, 0 );
        objects[i].unbindBuffers();
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


GLuint color_id(GLubyte pixel[4]) {
    return pixel[0] + (pixel[1]*256) + (pixel[2]*256*256);
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
GLint viewport[4];
int first_click = 1;
void
mouse( int button, int state, int x, int y ) {
    // Viewport and selected pixel
    GLubyte pixel[4];

    if (state == GLUT_DOWN) {

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        glUniform1i( swap_colors, 1 );

        // Render each loaded object file.
        for (unsigned int i = 0; i < objects.size(); ++i) {
            objects[i].bindBuffers();
            glUniformMatrix4fv(transformation, 1, GL_TRUE, objects[i].transform);
            glDrawElements( GL_TRIANGLES, objects[i].faces.size(), GL_UNSIGNED_INT, 0 );
            objects[i].unbindBuffers();
        }

        glGetIntegerv(GL_VIEWPORT, viewport);
        glReadPixels(x, viewport[3]-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

        // id corresponds to a specific vertex. That vertex is part of an
        // object. Set wireframe_vao to colorId of clicked object.
        wireframe_vao = color_id(pixel);
        for (unsigned int i = 0; i < objects.size(); ++i) {
            if (wireframe_vao == objects[i].objColor) {
                wireframe_vao = i;
                first_click = 1;
            }
        }

        // Don't swap buffers, just render over what's there.
        // Swapping buffers produces a flicker.
        glutPostRedisplay();
    }
}

/*
 * Handle dragging of mouse for transformations
 */
int start_pos[2] = {-1, -1};
void
mouseMotion(int x, int y) {
    if (first_click == 1) {
        // record mouse start position
        start_pos[0] = x;
        start_pos[1] = y;
        first_click = 0;
    } else {
        GLint dx =  (x - start_pos[0]);
        GLint dy = -(y - start_pos[1]);

        glGetIntegerv(GL_VIEWPORT, viewport);

        GLint w = viewport[2];
        GLint h = viewport[3];

        for (unsigned int i = 0; i < objects.size(); ++i) {
            if (wireframe_vao == i) {
                objects[i].transform *= Angel::Translate(
                    (GLfloat) dx/w*4,
                    (GLfloat) dy/h*4,
                    0.0);
            }
        }
        start_pos[0] = x;
        start_pos[1] = y;
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
        ObjParser* ps = new ObjParser(argv[i]);
        objects.push_back(*ps);
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
    glutMotionFunc(mouseMotion);
    glutDisplayFunc(display);
    if (ss.lens.size() == 4) {
        glutReshapeFunc(myPerspectiveReshape);
    } else {
        glutReshapeFunc(myOrthoReshape);
    }

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();

    return(0);
}
