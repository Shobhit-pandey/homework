//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "Angel.h"
#include "objParser.h"
#include <stdio.h>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection; // projection matrix uniform shader variable location

ParserState ps;

int
vec_size(std::vector<vec4> v) {
    return sizeof(v[0])*v.size();
}

int
vec_size_int(std::vector<int> v) {
    return sizeof(v[0])*v.size();
}

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


    // Sphere
    //mat4 p = Perspective(90.0, 1.0, 0.1, 8.0);
    //point4  eye( 0.0, 0.0, 2.0, 1.0);
    //point4  at( 0.0, 0.0, 0.0, 1.0 );
    //vec4    up( 0.0, 1.0, 0.0, 0.0 );

    // Teapot
    mat4 p = Perspective(45.0, 1.0, 0.1, 10.0);
    point4  eye( 0.0, 0.0, 2.0, 1.0);
    point4  at( 0.0, 0.0, 0.0, 1.0 );
    vec4    up( 0.0, 1.0, 0.0, 0.0 );

    // Teapot & Bunny
    //mat4 p = Perspective (60.0, 1.0, 0.1, 15.0);
    //point4  eye( 0.0, 0.0, 0.3, 1.0);
    //point4  at( 0.0, 1.0, 0.0, 1.0 );
    //vec4    up( 0.0, 1.0, 0.0, 0.0 );


    mat4  mv = LookAt( eye, at, up );

    glUniformMatrix4fv( model_view, 1, GL_TRUE, mv );
    glUniformMatrix4fv( projection, 1, GL_TRUE, p );


    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDrawElements( GL_TRIANGLES, ps.vertices.size()*6, GL_UNSIGNED_INT, 0 );
    glutSwapBuffers();
}

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
    }
}


int main(int argc, char** argv)
{
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

    // Parse obj
    //parse(&ps, "../assets/sphere42NS.obj");
    parse(&ps, "../assets/teapotNS.obj");
    //parse(&ps, "../assets/bunnyNS.obj");

    // Debug
    //printParserState(&ps);

    // Pass to init
    init(&ps);

    //NOTE:  callbacks must go after window is created!!!
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();

    return(0);
}
