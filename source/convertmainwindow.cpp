#include "convertmainwindow.h"
#include "ui_convertmainwindow.h"

#include <QPainter>
#include <QLabel>

ConvertMainWindow::ConvertMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConvertMainWindow)
{
    ui->setupUi(this);

    QImage img("images/1.jpg");

    QLabel *label = new QLabel(this);
    label->setText("ema");
    label->setPixmap(QPixmap::fromImage(img));

    ui->hLayout->addWidget(label);

    QImage temp(img);
    QRgb rgb;
    for (int x = 0; x < temp.width(); x++)
    {
        for (int y = 100; y < 200; y++)
        {
            rgb = qRgb(255, 0, 0);
            temp.setPixel(x, y, rgb);
        }
    }

    temp.save("images/result.jpg","JPG");
}

ConvertMainWindow::~ConvertMainWindow()
{
    delete ui;
}
