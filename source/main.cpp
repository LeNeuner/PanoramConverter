#include "convertmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ConvertMainWindow w;
    w.setWindowTitle("PanoramConverter");
    w.show();

    return a.exec();
}
