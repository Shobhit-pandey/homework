// SimpleProgram.cpp
//
// A simple 2D OpenGL program
//
// Assignment: 1
// Class: CS450 - Introduction to Computer Graphics
// Author: Trevor Bramwell
// Date: 10 October 2014
//

#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "../include/Angel.h"

// A global constant for the number of points that will be in each of
// the object.
const int TriPoints = 3;
const int RectPoints = 6;
const int CoorPoints = 4;
const int PointPoints = 5;

// Lines for the R2 Coordinate Plane
vec2 coor_plane[] = {
    vec2(-1, 0),
    vec2(1, 0),
    vec2(0, -1),
    vec2(0, 1)
};

// Colors for the R2 Coordinate Plane
vec4 coor_colors[] = {
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0)
};

// Some Random Points within R2
vec2 points[] = {
    vec2(-0.25, 0.75),
    vec2(0.75, 0.25),
    vec2(0.25, -0.25),
    vec2(0.25, 0.25),
    vec2(-0.25, -0.25),
};

// All points are white so they show up on the black background and
// multi-colored parallelogram
vec4 point_colors[] = {
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0)
};

// Right Triangle - Left side of Parallelogram
vec2 triangle[] = {
    vec2(-1, -0.5),
    vec2(-0.5, 0.5),
    vec2(-0.5, -0.5)
};

// Triangle Colors
vec4 tri_colors[] = {
    vec4(0.0, 1.0, 1.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(1.0, 0.0, 0.0, 1.0)
};

// Right Triangle - Right side of Parallelogram
vec2 triangle2[] = {
    vec2(0.5, 0.5),
    vec2(1, 0.5),
    vec2(0.5, -0.5)
};

// Right side triangle colors
vec4 tri2_colors[] = {
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(1.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 0.0, 1.0)
};

// Center rectangle of Parallelogram
vec2 rectangle[] = {
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, -0.5)
};

// Vertex colors of rectangle
vec4 rect_colors[] = {
    vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 1.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.0, 0.0, 1.0),
    vec4(1.0, 0.0,0.0, 1.0)
};

// Create multiple vertex array objects
GLuint vao[5];

// Create multiple vertex buffer objects
GLuint buffer[5];

void init(void)
{
    //
    // This program uses a seperate VAO and VBO for each primative it
    // renders. Though this is not at all efficient, it simplifies 
    // the glBufferData calls such that they don't run on, and on.
    //

    //
    // ---- SHADERS ----
    //

    // Initialize a vertex and fragment shader.
    GLuint program = InitShader("vshader32.glsl", "fshader32.glsl");
    // Use the shaders. This call could come before glDraw* to switch
    // out shaders.
    glUseProgram(program);
    // Get a pointer to the vPosition and vColor shader attributes
    GLuint pos = glGetAttribLocation(program, "vPosition");
    GLuint col = glGetAttribLocation(program, "vColor");

    // Generate multiple vertex array {objects,buffers}
    glGenVertexArrays(5, vao);
    glGenBuffers(5, buffer);

    //
    // ---- First Triangle (Left Side) ----
    //

    // Bind the first vertex array object and buffer.
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    // Set aside enought memory for the points and colors.
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle)+sizeof(tri_colors), NULL, GL_STATIC_DRAW);
    // Load the point data
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangle), triangle);
    // Load the color data
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(triangle), sizeof(tri_colors), tri_colors);
    // Tell OpenGL to use the vPosition attribute for this VAO
    glEnableVertexAttribArray(pos);
    // Let it know that the vPosition should read in vec2s from the
    // VBO.
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    // Let the vertex shader know to look for vec4s from the VBO at
    // offset sizeof(triangle).
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(triangle)));

    // 
    // ---- Second Triangle (Right Side) ----
    //
    //
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2)+sizeof(tri2_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangle2), triangle2);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(triangle2), sizeof(tri2_colors), tri2_colors);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(triangle2)));

    // 
    // ---- Rectangle ----
    //

    glBindVertexArray(vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle)+sizeof(rect_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectangle), rectangle);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(rectangle), sizeof(rect_colors), rect_colors);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(rectangle)));

    //
    // ---- Lines ----
    //

    glBindVertexArray(vao[3]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coor_plane)+sizeof(coor_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(coor_plane), coor_plane);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(coor_plane), sizeof(coor_colors), coor_colors);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(coor_plane)));

    //
    // ----- Points ----
    //

    glBindVertexArray(vao[4]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points)+sizeof(point_colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(point_colors), point_colors);
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(col);
    glVertexAttribPointer(col, 4, GL_FLOAT, GL_FALSE, 0,  BUFFER_OFFSET(sizeof(points)));

    // Make the background black
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // Ensure we don't try to implicitly draw something and fail.
    glBindVertexArray(0);
}

void
display(void)
{
    // Clear the window
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a Triangle
    glBindVertexArray(vao[0]);
    glDrawArrays(GL_TRIANGLES, 0, TriPoints);

    // Draw another Triangle
    glBindVertexArray(vao[1]);
    glDrawArrays(GL_TRIANGLES, 0, TriPoints);

    // Draw a Rectangle
    glBindVertexArray(vao[2]);
    glDrawArrays(GL_TRIANGLES, 0, RectPoints);

    // Draw the R2 Coordinate Plane
    glBindVertexArray(vao[3]);
    glDrawArrays(GL_LINES, 0, CoorPoints);

    // Draw some points in R2
    glBindVertexArray(vao[4]);
    glDrawArrays(GL_POINTS, 0, PointPoints);

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
