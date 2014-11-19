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
GLint new_axis = -1;

enum Axis {
    X = 1,
    Y,
    Z
};

enum Mode {
    Translate = 1,
    Rotate,
    Scale
};

Axis axis = X;
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
                manipulator[j].draw();
                manipulator[j].swapColors(0);
                manipulator[j].translate = objects[i].translate;
            }
        } else {
            objects[i].draw();
        }
    }


    glutSwapBuffers();
}


GLuint color_id(vec4 pixel) {
    return (GLuint) ((pixel[0]*255.0f) + (pixel[1]*255.0f)*256 + (pixel[2]*255.0f)*256*256);
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
    GLfloat pixel[4];

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
        // Render manipulators
        for (unsigned int j = 0; j < manipulator.size(); ++j) {
            manipulator[j].swapColors(1);
            manipulator[j].draw();
            manipulator[j].swapColors(0);
            if (wireframe_vao < objects.size()) {
                manipulator[j].translate = objects[wireframe_vao].translate;
            }
        }

        glGetIntegerv(GL_VIEWPORT, viewport);
        glReadPixels(x, viewport[3]-y, 1, 1, GL_RGBA, GL_FLOAT, pixel);

        vec4 selected_color(pixel[0], pixel[1], pixel[2], 1.0);

        // Clicked an Axis
        new_axis = -1;
        for (unsigned int j = 0; j < manipulator.size(); ++j) {
            if (color_id(selected_color) == color_id(manipulator[j].objColor)) {
                new_axis = (j+1);
            }
        }
        // Selected an axis
        if (new_axis == -1) {
            wireframe_vao = -1;
        } else {
            switch (new_axis) {
                case 1: axis = X; break;
                case 2: axis = Y; break;
                case 3: axis = Z; break;
            }
        }
        // Clicked an object
        for (unsigned int i = 0; i < objects.size(); ++i) {
            if (color_id(selected_color) == color_id(objects[i].objColor)) {
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
    GLint dx = (x - start_pos[0]);
    GLint dy = (start_pos[1] - y);

    glGetIntegerv(GL_VIEWPORT, viewport);

    //vec4 invert_z( 1.0, 1.0, -1.0, 0.0);
    vec4 tvec(dx, dy, 0.0, 0.0);

    vec4 world_vec = inverse(mv) * tvec;

    for (unsigned int i = 0; i < objects.size(); ++i) {
        if (wireframe_vao == i && new_axis != -1) {
            switch (mode) {
            case Translate:
                switch (axis) {
                case X:
                    objects[i].translate *= Angel::Translate(
                        0.01f * world_vec * vec3(1.0, 0.0, 0.0) // X-mask
                    );
                    break;
                case Y:
                    objects[i].translate *= Angel::Translate(
                        0.01f * world_vec * vec3(0.0, 1.0, 0.0) // Y-mask
                    );
                    break;
                case Z:
                    objects[i].translate *= Angel::Translate(
                        0.01f * world_vec * vec3(0.0, 0.0, -1.0) // Z-mask
                    );
                    break;
                }
                for (unsigned int j = 0; j < manipulator.size(); ++j) {
                    manipulator[j].translate = objects[i].translate;
                }
                break;
            case Rotate:
                switch (axis) {
                case X:
                    objects[i].rotate = Angel::RotateX(
                        world_vec[0]
                    ) * objects[i].rotate;
                    break;
                case Y:
                    objects[i].rotate = Angel::RotateY(
                        world_vec[1]
                    ) * objects[i].rotate;
                    break;
                case Z:
                    objects[i].rotate = Angel::RotateZ(
                        world_vec[2]
                    ) * objects[i].rotate;
                    break;
                }
                break;
            case Scale:
                switch (axis) {
                case X:
                    objects[i].scale = Angel::Scale(
                        vec3(1.0+(world_vec[0]*1.50f), 1.0, 1.0)
                    ) + objects[i].scale;
                    break;
                case Y:
                    objects[i].scale = Angel::Scale(
                        vec3(1.0, 1.0+(world_vec[1]*1.50f), 1.0)
                    ) + objects[i].scale;
                    break;
                case Z:
                    objects[i].scale = Angel::Scale(
                        vec3(1.0, 1.0, -(world_vec[2]*1.50f))
                    ) + objects[i].scale;
                    break;
                }
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
    switch( key ) {
    // States
	case 033:  // Escape key
	case 'Q': exit( EXIT_SUCCESS ); break;
    case '5': printSceneState(&ss); break;
    // At
    case '4': ss.at[0] -= 0.10; break;
    case '6': ss.at[0] += 0.10; break;
    case '2': ss.at[1] -= 0.10; break;
    case '8': ss.at[1] += 0.10; break;
    case '7': ss.at[2] += 0.10; break;
    case '9': ss.at[2] -= 0.10; break;
    // Eye
    case 'w': ss.eye[2] -= 0.10; break;
    case 's': ss.eye[2] += 0.10; break;
    case 'a': ss.eye[0] -= 0.10; break;
    case 'd': ss.eye[0] += 0.10; break;
    case 'q': ss.eye[1] -= 0.10; break;
    case 'e': ss.eye[1] += 0.10; break;
    // Up
    case 'i': ss.up[0]  += 0.10; break;
    case 'k': ss.up[0]  -= 0.10; break;
    case 'j': ss.up[1]  += 0.10; break;
    case 'l': ss.up[1]  -= 0.10; break;
    case 'o': ss.up[2]  += 0.10; break;
    case 'u': ss.up[2]  -= 0.10; break;
    // Transformations
    case 't': mode = Translate; break;
    case 'g': mode = Scale; break;
    case 'r': mode = Rotate; break;
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
