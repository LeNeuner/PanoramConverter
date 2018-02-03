#ifndef CONVERTMAINWINDOW_H
#define CONVERTMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ConvertMainWindow;
}

class ConvertMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConvertMainWindow(QWidget *parent = 0);
    ~ConvertMainWindow();

private slots:
    void on_bLoad_clicked();
    void on_bSave_clicked();
    void on_bConvert_clicked();

private:
    Ui::ConvertMainWindow   *ui;
    QImage                  *srcImage   = nullptr;
    QImage                  *dstImage   = nullptr;
    int                     statBarTout = 2000;
};

#endif // CONVERTMAINWINDOW_H
