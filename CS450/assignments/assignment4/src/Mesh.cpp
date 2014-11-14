#include <stdio.h>
#include <string.h>
#include <vector>

#include "Angel.h"
#include "ObjParser.h"
#include "Mesh.h"

using Angel::vec4;
using std::vector;

Mesh::Mesh(const char* objFilename) {
    ps = new ObjParser(objFilename);
    genColors();
    setupBuffers();
}

Mesh::~Mesh() {}

void Mesh::setupBuffers() {
    // Generate buffers
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // vao
    glBindVertexArray(vao);

    // Determine sizes for buffer offsets.
    GLsizeiptr verticesSize = sizeof(vec4) * ps->vertices.size();
    GLsizeiptr normalsSize = sizeof(vec4) * ps->normals.size();
    GLsizeiptr colorsSize = sizeof(vec4) * colors.size();
    GLsizeiptr facesSize = sizeof(GLuint) * ps->faces.size();

    // vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize + colorsSize + normalsSize, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, ps->vertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, verticesSize, colorsSize, colors.data());
    glBufferSubData(GL_ARRAY_BUFFER, verticesSize + colorsSize, normalsSize, ps->normals.data());

    // ebo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesSize, ps->faces.data(), GL_STATIC_DRAW);

    unbindBuffers();
}

void Mesh::setupShaders(GLuint program) {
    shader_program = program;
    glUseProgram(program);

    bindBuffers();

    // vPosition
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    // vColor
    GLint64 verticesSize = sizeof(Angel::vec4) * ps->vertices.size();
    GLuint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           0,
                           BUFFER_OFFSET((verticesSize)) );
    // vNormal
    GLint64 colorsSize = sizeof(Angel::vec4) * colors.size();
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal,
                           4,
                           GL_FLOAT,
                           GL_FALSE,
                           0,
                           BUFFER_OFFSET(verticesSize + colorsSize) );


    // Initialize shader lighting parameters
    // RAM: No need to change these...we'll learn about the details when we
    // cover Illumination and Shading
    vec4 light_position( 1.5, 1.5, 2.0, 1.0 );
    vec4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
    vec4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
    vec4 light_specular( 1.0, 1.0, 1.0, 1.0 );

    vec4 material_ambient( 1.0, 0.0, 1.0, 1.0 );
    vec4 material_diffuse( 1.0, 0.8, 0.0, 1.0 );
    vec4 material_specular( 1.0, 0.8, 0.0, 1.0 );
    float  material_shininess = 100.0;

    vec4 ambient_product = light_ambient * material_ambient;
    vec4 diffuse_product = light_diffuse * material_diffuse;
    vec4 specular_product = light_specular * material_specular;

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

    unbindBuffers();
}

void Mesh::wireframe() {
    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(1.0, 2.0);

    // Draw the object
    draw();
    
    // Change back to normal GL_FILL mode.
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void Mesh::draw() {
    glUseProgram(shader_program);

    bindBuffers();

    // Apply transformations
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "Transform" ),
            1, GL_TRUE, transform());

    glDrawElements(GL_TRIANGLES, ps->faces.size(), GL_UNSIGNED_INT, 0);

    unbindBuffers();
}

Angel::mat4 Mesh::transform() {
    return translate * scale * rotate;
}

void Mesh::swapColors(int swap) {
    glUniform1i(glGetUniformLocation( shader_program, "Swap" ),
            swap);
}

void Mesh::bindBuffers() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

void Mesh::unbindBuffers() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::genColors() {
    // Generate a random color for each vertex using colorId, assigning it to objColor.
    for (unsigned int i = 0; i < ps->vertices.size(); ++i) {
        GLuint r = (colorId & 0x000000FF) >>  0;
        GLuint g = (colorId & 0x0000FF00) >>  8;
        GLuint b = (colorId & 0x00FF0000) >> 16;

        vec4 unique_color(r/255.0f, g/255.0f, b/255.0f, 1.0);
        colors.push_back(unique_color);
    }

    objColor = colorId;
    ++colorId;
}
