#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>

// структура данных вершины
struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};

GeometryEngine::GeometryEngine()
    : indexBuf(QOpenGLBuffer::IndexBuffer)
{
    // инициализация функций openGL
    initializeOpenGLFunctions();

    // генерация 2х вершинных буферов (методов) VBO
    arrayBuf.create();
    indexBuf.create();

    // инициализация геометрии куба и трансляция в буферы VBO
    initCubeGeometry();
}

GeometryEngine::~GeometryEngine()
{
    arrayBuf.destroy();
    indexBuf.destroy();
}

void GeometryEngine::initCubeGeometry()
{
    // связь координат вершин на кубе и  на тектстуре
    VertexData vertices[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.25f, 0.33f)}, // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.5f,  0.33f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.25f, 0.66f)}, // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.5f,  0.66f)}, // v3

        // Vertex data for face 1
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.5f,  0.33f)}, // v4
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.75f, 0.33f)}, // v5
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.5f,  0.66f)}, // v6
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.75f, 0.66f)}, // v7

        // Vertex data for face 2
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.75f, 0.33f)}, // v8
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(1.0f,  0.33f)}, // v9
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.75f, 0.66f)}, // v10
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(1.0f,  0.66f)}, // v11

        // Vertex data for face 3
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.0f,  0.33f)}, // v12
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.25f, 0.33f)}, // v13
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.0f,  0.66f)}, // v14
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.25f, 0.66f)}, // v15

        // Vertex data for face 4
        {QVector3D(-1.0f, -1.0f, -1.0f), QVector2D(0.25f, 0.0f)},  // v16
        {QVector3D( 1.0f, -1.0f, -1.0f), QVector2D(0.5f,  0.0f)},  // v17
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.25f, 0.33f)}, // v18
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.5f,  0.33f)}, // v19

        // Vertex data for face 5
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.25f, 0.66f)}, // v20
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.5f,  0.66f)}, // v21
        {QVector3D(-1.0f,  1.0f, -1.0f), QVector2D(0.25f, 1.0f)},  // v22
        {QVector3D( 1.0f,  1.0f, -1.0f), QVector2D(0.5f,  1.0f)}   // v23
    };

    // инициализация порядка отрисовки треугольников
    GLushort indices[] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    // передача вершин в VBO 0
    arrayBuf.bind();
    arrayBuf.allocate(vertices, 24 * sizeof(VertexData));

    // передача вершин в VBO 1
    indexBuf.bind();
    indexBuf.allocate(indices, 34 * sizeof(GLushort));
}

// отрисовка куба
void GeometryEngine::drawCubeGeometry(QOpenGLShaderProgram *program)
{
    // связь буферов VBO с OpenGL
    arrayBuf.bind();
    indexBuf.bind();

    // смещение
    quintptr offset = 0;

    // информация о положении вершин программируемому конвееру OpenGL
    int vertexLocation = program->attributeLocation("a_position");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(VertexData));

    // смещение для координат текстуры
    offset += sizeof(QVector3D);

    // информация о положении координат вершин на текстуре программируемому конвееру OpenGL
    int texcoordLocation = program->attributeLocation("a_texcoord");
    program->enableAttributeArray(texcoordLocation);
    program->setAttributeBuffer(texcoordLocation, GL_FLOAT, offset, 2, sizeof(VertexData));

    // отрисовка куба с использованием индексов из VBO 1
    glDrawElements(GL_TRIANGLE_STRIP, 34, GL_UNSIGNED_SHORT, 0);
}
