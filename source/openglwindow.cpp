#include "openglwindow.h"
#include "ui_openglwindow.h"

#include <QLabel>
#include <QSurfaceFormat>

#ifndef QT_NO_OPENGL
#include "mainwidget.h"
#endif

OpenGLWindow::OpenGLWindow(QImage *srcImage, QWidget *parent) :
    QMainWindow(parent),
    image(srcImage),
    ui(new Ui::OpenGLWindow)
{
    ui->setupUi(this);

    // инициализация формата для вывода 3D
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

#ifndef QT_NO_OPENGL
    MainWidget* widget = new MainWidget(image);
    setCentralWidget(widget);
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
}

OpenGLWindow::~OpenGLWindow()
{
    delete ui;
}
