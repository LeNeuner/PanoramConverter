#include "mainwidget.h"

#include <QMouseEvent>
#include <math.h>

MainWidget::MainWidget(QImage *srcImage, QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    image(srcImage),
    source(0),
    angularSpeed(0)
{
}

MainWidget::~MainWidget()
{
    makeCurrent();
    delete source;
    delete geometries;
    doneCurrent();
}

void MainWidget::mousePressEvent(QMouseEvent *e)
{
    // сохранить позицию нажатия мыши
    mousePressPosition = QVector2D(e->localPos());
}

void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
    // положение выключения мыши - положение нажатия мыши
    QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

    // ось вращения перпендикулярна разности положения мыши
    QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

    // ускорение скорости относительно длины развертки мыши
    qreal acc = diff.length() / 100.0;

    // расчет новой оси вращения
    rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

    // увеличение угловой скорости
    angularSpeed += acc;
}

void MainWidget::timerEvent(QTimerEvent *)
{
    // постепенное уменьшение угловой скорости
    angularSpeed *= 0.99;

    // остановить вращение, если скорость ниже порога
    if (angularSpeed < 0.01)
    {
        angularSpeed = 0.0;
    }
    else
    {
        // обновить вращение
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;
        update();
    }
}

void MainWidget::initializeGL()
{
    // инициализация функций openGL
    initializeOpenGLFunctions();

    // очистка
    glClearColor(0, 0, 0, 1);

    // инициализация шейдеров и текстур
    initShaders();
    initTextures();

    // включение буфера глубины
    glEnable(GL_DEPTH_TEST);

    // включить отбрасывание задней поверхности
    glEnable(GL_CULL_FACE);

    // иницилизация движка
    geometries = new GeometryEngine;
    timer.start(12, this);
}

void MainWidget::initShaders()
{
    // компиляция вершинного шейдера
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // компиляция фрагментарного (пиксельного) шейдера
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // связь с шейдерным конвеером
    if (!program.link())
        close();
    if (!program.bind())
        close();
}

void MainWidget::initTextures()
{
    // загрузка изображения
    source = new QOpenGLTexture(image->mirrored());
    destination = new QOpenGLTexture(QImage(source->width(), source->width() * 3 / 4, QImage::Format_RGB32));

    // установить режим фильтрации по принципу "ближайшего соседа" для уменьшения текстуры
    destination->setMinificationFilter(QOpenGLTexture::Nearest);

    // установить режим билинейной фильтрации для увеличения текстуры
    destination->setMagnificationFilter(QOpenGLTexture::Linear);

    // увеличение (обтекание) текстуры путем ее повторения
    destination->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::resizeGL(int w, int h)
{
    // расчет соотношения сторон
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // установка ближней поскости, дальней плоскости и поля зрения
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // сброс проекции
    projection.setToIdentity();

    // установка перспективной проекции
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL()
{
    // очистка цвета и буфера глубины
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // активировать в слоты две текстуры для передачи в шейдер
    glActiveTexture(GL_TEXTURE0);
    destination->bind();
    glActiveTexture(GL_TEXTURE1);
    source->bind();

    // расчет матрицы представления/трансформации модели
    QMatrix4x4 matrix;
    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    // установить матрицу представления модели
    program.setUniformValue("mvp_matrix", projection * matrix);

    // назначение iniform значений
    program.setUniformValue("destination", 0);
    program.setUniformValue("source", 1);

    // отрисовка куба
    geometries->drawCubeGeometry(&program);
}
