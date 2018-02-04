#include "convertmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConvertMainWindow w;
    w.setWindowTitle("PanoramConverter");
    w.setWindowIcon(QIcon(":AppIcon"));
    w.setMinimumHeight(400);
    w.setMinimumWidth(800);
    w.show();

    return a.exec();
}
