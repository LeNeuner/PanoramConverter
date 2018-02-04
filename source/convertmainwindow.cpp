#include "convertmainwindow.h"
#include "ui_convertmainwindow.h"

#include <QPainter>
#include <QLabel>
#include <QFileDialog>
#include <QDebug>

#include "imageconverter.h"

ConvertMainWindow::ConvertMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConvertMainWindow)
{
    ui->setupUi(this);

    // инициализация изображений
    srcImage = new QImage();
    dstImage = new QImage();
}

ConvertMainWindow::~ConvertMainWindow()
{
    delete ui;
}


void ConvertMainWindow::on_bLoad_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                    QDir::currentPath(),  tr("Image Files (*.png *.jpg *.bmp)"));
    bool loadRes = false;
    if ((!filename.isEmpty()) && (!filename.isNull()))
    {
        loadRes = srcImage->load(filename);

        if (loadRes)
        {
            qDebug() << "img size:  " << srcImage->size();

            ui->lblSrcImage->setPixmap(QPixmap::fromImage(*srcImage));
            ui->lblSrcImage->setScaledContents(true);

            // масштабирование больших изображений временно так
            int w = srcImage->width();
            int h = srcImage->height();
            double ratio = w / h;
            int maxWidth = 400;

            qDebug() << "w:  " << w;
            qDebug() << "h:  " << h;

            if (w > 400)
            {
                ui->lblSrcImage->setFixedWidth(maxWidth);
                ui->lblSrcImage->setFixedHeight(abs(static_cast<int>(maxWidth/ratio)));
            }
        }
    }

    if (loadRes)
        this->statusBar()->showMessage("Изображение успешно загружено", statBarTout);
    else
        this->statusBar()->showMessage("Ошибка загрузки изображения",   statBarTout);

    *dstImage = QImage(srcImage->width(), srcImage->width() * 3 / 4, QImage::Format_RGB32);
    dstImage->fill(Qt::black);
}

void ConvertMainWindow::on_bSave_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save image"),
                                    QDir::currentPath(),  tr("Image Files (*.png *.jpg *.bmp)"));
    bool saveRes = false;
    if ((!filename.isEmpty()) && (!filename.isNull()))
    {
        saveRes = dstImage->save(filename);
    }

    if (saveRes)
        this->statusBar()->showMessage("Изображение успешно сохранено", statBarTout);
    else
        this->statusBar()->showMessage("Ошибка сохранения изображения", statBarTout);
}

void ConvertMainWindow::on_bConvert_clicked()
{
    // конвертирование изображения
    ImageConverter* converter = new ImageConverter(srcImage, dstImage);

    // отображение прогресса обработки
    connect(converter, SIGNAL(needProgressChange(int)),
            this, SLOT(setProgress(int)));

    ui->lblDstImage->setPixmap(QPixmap::fromImage(*dstImage));
    ui->lblDstImage->setScaledContents(true);

    // масштабирование больших изображений временно так
    int w = dstImage->width();
    int h = dstImage->height();
    double ratio = w / h;
    int maxWidth = 400;

    qDebug() << "w:  " << w;
    qDebug() << "h:  " << h;

    if (w > 400)
    {
        ui->lblDstImage->setFixedWidth(maxWidth);
        ui->lblDstImage->setFixedHeight(abs(static_cast<int>(maxWidth/ratio)));
    }

}

void ConvertMainWindow::setProgress(int val)
{
    ui->progressBar->setValue(val);
}
