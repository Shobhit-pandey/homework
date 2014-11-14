//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

#define WINDOW_WIDTH  512
#define WINDOW_HEIGHT 512
#define UNIT_CUBE "unit_cube.obj"

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "Angel.h"
#include "Mesh.h"
#include "sceneParser.h"
#include <stdio.h>

GLuint Mesh::colorId = 0;

using scene::SceneState;

using Angel::vec4;
using Angel::mat4;
using Angel::Perspective;
using Angel::DegreesToRadians;
using Angel::Ortho;
using Angel::Frustum;
using Angel::InitShader;
using Angel::inverse;

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

mat4 mv;

GLuint program;

GLuint wireframe_vao = -1;

enum Mode {
    Translate = 1,
    RotateX,
    RotateY,
    RotateZ,
    Scale
};

Mode mode = Translate;

// SceneState hold viewing parameters
SceneState ss;

// Mesh hold parsed objects
std::vector<Mesh> objects;
std::vector<Mesh> manipulator;


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


void
display( void )
{
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Update Camera
    point4 eye( ss.eye, 1.0 );
    point4  at( ss.at, 1.0 );
    vec4    up( ss.up, 0.0 );

    mv = LookAt( eye, at, up );

    glUniformMatrix4fv( glGetUniformLocation( program, "ModelView" ),
            1, GL_TRUE, mv );
    glUniformMatrix4fv( glGetUniformLocation( program, "Projection" ),
            1, GL_TRUE, ss.proj );

    // Render each loaded object file.
    for (unsigned int i = 0; i < objects.size(); ++i) {
        if (wireframe_vao == i) {
            objects[i].wireframe();
            // Draw manipulator
            for (unsigned int j = 0; j < manipulator.size(); ++j) {
                manipulator[j].swapColors(1);
                manipulator[j].translate = objects[i].translate;
                manipulator[j].draw();
                manipulator[j].swapColors(0);
            }
        } else {
            objects[i].draw();
        }
    }


    glutSwapBuffers();
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
int start_pos[2];
void
mouse( int button, int state, int x, int y ) {
    // Viewport and selected pixel
    GLubyte pixel[4];

    if (state == GLUT_DOWN) {
        start_pos[0] = x;
        start_pos[1] = y;

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Render each loaded object file.
        for (unsigned int i = 0; i < objects.size(); ++i) {
            objects[i].swapColors(1);
            objects[i].draw();
            objects[i].swapColors(0);
        }

        glGetIntegerv(GL_VIEWPORT, viewport);
        glReadPixels(x, viewport[3]-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

        // id corresponds to a specific vertex. That vertex is part of an
        // object. Set wireframe_vao to colorId of clicked object.
        wireframe_vao = color_id(pixel);
        for (unsigned int i = 0; i < objects.size(); ++i) {
            if (wireframe_vao == objects[i].objColor) {
                wireframe_vao = i;
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
void
mouseMotion(int x, int y) {
    GLint dx =  (x - start_pos[0]);
    GLint dy = -(y - start_pos[1]);

    glGetIntegerv(GL_VIEWPORT, viewport);

    GLint w = viewport[2];
    GLint h = viewport[3];

    //printf("diff: (%f, %f)\n",
    //        (GLfloat) dx/w,
    //        (GLfloat) dy/h);

    vec4 t_vec( (GLfloat) dx/w*4, (GLfloat) dy/h*4, 0.0, 1.0);

    vec4 invert_z( 1.0, 1.0, -1.0, 1.0);

    for (unsigned int i = 0; i < objects.size(); ++i) {
        if (wireframe_vao == i) {
            switch (mode) {
            case Translate:
                objects[i].translate *= Angel::Translate(
                    inverse(mv) * t_vec * invert_z
                );
                break;
            case RotateX:
                objects[i].rotate *= Angel::RotateX((GLfloat) dy/2);
                break;
            case RotateY:
                objects[i].rotate *= Angel::RotateY((GLfloat) dx/2);
                break;
            case RotateZ:
                objects[i].rotate *= Angel::RotateZ((GLfloat) dx+dy/2);
                break;
            case Scale:
                //GLfloat scaleX = objects[i].transform[0][0];
                //GLfloat scaleY = objects[i].transform[1][1];
                //objects[i].scale = Angel::Scale(
                //    scaleX + (GLfloat) (w-x)/w,
                //    scaleY + (GLfloat) dy/(h/2),
                //    1.0);
                break;
            }
        }
    }
    if (mode != Scale) {
        start_pos[0] = x;
        start_pos[1] = y;
    }

    glutPostRedisplay();
}


// Keys primarily control the camera parameters
void
keyboard( unsigned char key, int x, int y )
{
    if (key == 'x') {
        if (mode == Translate || mode == Scale) {
            mode = RotateX;
        } else if (mode == RotateX) {
            mode = RotateY;
        } else if (mode == RotateY) {
            mode = RotateZ;
        } else if (mode == RotateZ) {
            mode = RotateX;
        }
    }
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
    case 'z': mode = Translate; break;
    case 'c': mode = Scale; break;
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
        Mesh mesh(argv[i]);
        objects.push_back(mesh);
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

    // Initialize Shaders
    program = InitShader("vshader.glsl", "fshader.glsl");

    // Pass objects to init
    for (unsigned int i = 0; i < objects.size(); ++i) {
        objects[i].setupShaders(program);
    }

    // Create manipulators
    Mesh unit_x("unit_cube.obj", vec4(1.0, 0.0, 0.0, 1.0));
    Mesh unit_y(UNIT_CUBE, vec4(0.0, 1.0, 0.0, 1.0));
    Mesh unit_z(UNIT_CUBE, vec4(0.0, 0.0, 1.0, 1.0));

    // Scale
    unit_x.scale = unit_y.scale = unit_z.scale = Angel::Scale(0.5, 0.5, 0.5);

    // Translate
    unit_x.offset *= Angel::Translate(vec4(0.75, 0.0, 0.0, 0.0));
    unit_y.offset *= Angel::Translate(vec4(0.0, 0.75, 0.0, 0.0));
    unit_z.offset *= Angel::Translate(vec4(0.0, 0.0, 0.75, 0.0));

    manipulator.push_back(unit_x);
    manipulator.push_back(unit_y);
    manipulator.push_back(unit_z);

    for (unsigned int i = 0; i < manipulator.size(); ++i) {
        manipulator[i].setupShaders(program);
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
