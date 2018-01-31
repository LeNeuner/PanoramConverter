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

private:
    Ui::ConvertMainWindow *ui;
};

#endif // CONVERTMAINWINDOW_H
