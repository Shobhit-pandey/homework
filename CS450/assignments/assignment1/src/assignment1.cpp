// SimpleProgram.cpp
//
// A simple 2D OpenGL program

#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "../include/Angel.h"

// A global constant for the number of points that will be in our object.
const int TriPoints = 3;
const int RectPoints = 6;

// Specifiy the vertices for a rectangle.  The first and last vertex are
// duplicated to close the box.
vec2 triangle[] = {
    vec2(-0.5, 0.5), // Triangle
    vec2(-1, -0.5),
    vec2(0, -0.5)
};

vec4 tri_colors[] = {
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(1.0, 0.0, 0.0, 1.0)
};

vec2 rectangle[] = {
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, -0.5)
};

vec4 rect_colors[] = {
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.0, 0.0, 1.0),
    vec4(1.0, 0.0,0.0, 1.0)
};

// Create two vertex array object---OpenGL needs this to manage the Vertex
// Buffer Object
GLuint vao[2];

// Create and initialize a buffer object---that's the memory buffer that
// will be on the card!
GLuint buffer[2];

void init(void)
{
    //
    // ---- SHADERS ----
    //
    GLuint program = InitShader("vshader32.glsl", "fshader32.glsl");
    glUseProgram(program);
    GLuint pos = glGetAttribLocation(program, "vPosition");
    GLuint col = glGetAttribLocation(program, "vColor");

    // Generate two vertex arrays and two buffers
    glGenVertexArrays(2, vao);
    glGenBuffers(2, buffer);

    // 
    // ---- Triangle ----
    //

    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle)+sizeof(tri_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangle), triangle);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(triangle), sizeof(tri_colors), tri_colors);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(triangle)));


    // 
    // ---- Rectangle ----
    //

    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle)+sizeof(rect_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectangle), rectangle);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(rectangle), sizeof(rect_colors), rect_colors);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(rectangle)));

    // Make the background white
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glBindVertexArray(0);
}

void
display(void)
{
    // clear the window
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Triangle
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, TriPoints);

    // Draw Rectangle
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, RectPoints);

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
