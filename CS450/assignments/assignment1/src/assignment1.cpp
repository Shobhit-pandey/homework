// SimpleProgram.cpp
//
// A simple 2D OpenGL program

#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "../include/Angel.h"

// A global constant for the number of points that will be in our object.
const int NumPoints = 9;

// Specifiy the vertices for a rectangle.  The first and last vertex are
// duplicated to close the box.
vec2 verticies[] = {
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.25), // Triangle
    vec2(-1, -0.5),
    vec2(0, -0.5)
};

// Create two vertex array object---OpenGL needs this to manage the Vertex
// Buffer Object
GLuint vao;

// Create and initialize a buffer object---that's the memory buffer that
// will be on the card!
GLuint buffer;

void init(void)
{

    // Generate two vertex arrays.
    glGenVertexArrays(1, &vao);

    // We need one for this example.
    glGenBuffers(1, &buffer);

    // 
    // ---- RECTAGLE ----
    //

    // Bind the first one to make it active
    glBindVertexArray(vao);

    // Bind makes it the active VBO
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    // Here we copy the vertex data into our buffer on the card.  The
    // parameters tell it the type of buffer object, the size of the
    // data in bytes, the pointer for the data itself, and a hint for
    // how we intend to use it.
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    //
    // ---- SHADERS ----
    //
    // Load the shaders.  Note that this function is not offered by OpenGL
    // directly, but provided as a convenience.
    GLuint program = InitShader("vshader32.glsl", "fshader32.glsl");

    // Make that shader program active.
    glUseProgram(program);

    // Initialize the vertex position attribute from the vertex shader.  When
    // the shader and the program are linked, a table is created for the shader
    // variables.  Here, we get the index of the vPosition variable.
    GLuint loc = glGetAttribLocation(program, "vPosition");

    // We want to set this with an array!
    glEnableVertexAttribArray(loc);

    // We map it to this offset in our current buffer (VBO) So, our position
    // data is going into loc and contains 2 floats.  The parameters to this
    // function are the index for the shader variable, the number of
    // components, the type of the data, a boolean for whether or not
    // this data is normalized (0--1), stride (or byte offset between
    // consective attributes), and a pointer to the first component.
    // Note that BUFFER_OFFSET is a macro defined in the Angel.h file.
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // Make the background white
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

void
display(void)
{
    // clear the window
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Rectagle and Triangle
    glBindVertexArray(vao);

    // Draw the points.  The parameters to the function are: the mode,
    // the first index, and the count.
    glDrawArrays(GL_TRIANGLES, 0, NumPoints);

    glFlush();
    glutSwapBuffers();
}

void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    // Quit when ESC is pressed
    case ESC_KEY:
        exit(EXIT_SUCCESS);
        break;
    }
}

// This program draws a red rectangle on a white background, but it's still
// missing the machinery to move to 3D.
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitContextVersion (3, 2);
    glutInitContextFlags (GLUT_FORWARD_COMPATIBLE);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(500, 300);
    glutCreateWindow("Simple Open GL Program");
    printf("%s\n%s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

    glewExperimental = GL_TRUE;
    glewInit();

    init();

    //NOTE:  callbacks must go after window is created!!!
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();

    return(0);
}
