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

Mesh::Mesh(const char* objFilename, Angel::vec4 color) {
    ps = new ObjParser(objFilename);
    setColor(color);
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

    glUniformMatrix4fv( glGetUniformLocation(program, "View"),
            1, GL_TRUE, ss->mv );
    glUniformMatrix4fv( glGetUniformLocation(program, "Projection"),
            1, GL_TRUE, ss->proj );
    // Apply transformations
    glUniformMatrix4fv(glGetUniformLocation(program, "Transform" ),
            1, GL_TRUE, transform());
}

void Mesh::wireframe(GLuint program) {
    // Enable wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonOffset(1.0, 2.0);

    // Draw the object
    draw(program);
    
    // Change back to normal GL_FILL mode.
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void Mesh::draw(GLuint program) {
    shader = program;
    glUseProgram(program);

    bindBuffers();

    setupShaders(program);

    glDrawElements(GL_TRIANGLES, ps->faces.size(), GL_UNSIGNED_INT, 0);

    unbindBuffers();
}

Angel::mat4 Mesh::transform() {
    return (translate * offset) * scale * rotate;
}

void Mesh::setColor(Angel::vec4 color) {
    for (unsigned int i = 0; i < ps->vertices.size(); ++i) {
        colors.push_back(color);
    }

    objColor = color;
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
    GLuint r = (colorId & 0x000000FF) >>  0;
    GLuint g = (colorId & 0x0000FF00) >>  8;
    GLuint b = (colorId & 0x00FF0000) >> 16;

    vec4 unique_color(r/255.0f, g/255.0f, b/255.0f, 1.0);

    for (unsigned int i = 0; i < ps->vertices.size(); ++i) {
        colors.push_back(unique_color);
    }

    objColor = unique_color;
    ++colorId;
}
