#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QMainWindow>

namespace Ui {
class OpenGLWindow;
}

class OpenGLWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OpenGLWindow(QImage *srcImage, QWidget *parent = 0);
    ~OpenGLWindow();

private:
    Ui::OpenGLWindow *ui;
    QImage *image;
};

#endif // OPENGLWINDOW_H
