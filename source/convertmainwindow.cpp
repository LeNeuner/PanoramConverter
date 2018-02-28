#include "convertmainwindow.h"
#include "ui_convertmainwindow.h"

#include <QPainter>
#include <QLabel>
#include <QFileDialog>
#include <QDebug>

#include "imageconverter.h"
#include "openglwindow.h"

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
    // получение пути к исходному изображению
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                    QDir::currentPath(),  tr("Image Files (*.png *.jpg *.bmp)"));
    // загрузка изображения
    bool loadRes = false;
    if ((!filename.isEmpty()) && (!filename.isNull()))
    {
        loadRes = srcImage->load(filename);
        if (loadRes)
        {
            // масштабирование больших изображений
            QSize imageSize = QSize(srcImage->width(), srcImage->height());
            QSize labelSize = ui->lblSrcImage->size();

            if (imageSize.height() > labelSize.height() || imageSize.width() > labelSize.width())
                imageSize.scale((labelSize), Qt::KeepAspectRatio);

            // вывод результатов загрузки
            ui->lblSrcImage->setPixmap(QPixmap::fromImage(*srcImage));
            ui->lblSrcImage->setScaledContents(true);

            // разблокировка кнопки расчета на GPU
            ui->bGPU->setEnabled(true);
        }
    }

    // обработка результатов загрузки
    if (loadRes)
        this->statusBar()->showMessage("Изображение успешно загружено", statBarTout);
    else
        this->statusBar()->showMessage("Ошибка загрузки изображения",   statBarTout);

    // установка формата выходного изображения в зависимости от формата входного
    *dstImage = QImage(srcImage->width(), srcImage->width() * 3 / 4, QImage::Format_RGB32);
    dstImage->fill(Qt::black);
}

void ConvertMainWindow::on_bSave_clicked()
{
    // получение пути для сохранения выходного изображения
    QString filename = QFileDialog::getSaveFileName(this, tr("Save image"),
                                    QDir::currentPath(),  tr("Image Files (*.png *.jpg *.bmp)"));
    // сохранение изображения
    bool saveRes = false;
    if ((!filename.isEmpty()) && (!filename.isNull()))
    {
        saveRes = dstImage->save(filename);
    }

    // обработка результатов сохранения
    if (saveRes)
        this->statusBar()->showMessage("Изображение успешно сохранено", statBarTout);
    else
        this->statusBar()->showMessage("Ошибка сохранения изображения", statBarTout);
}

void ConvertMainWindow::on_bConvert_clicked()
{
    // создание конвертора
    ImageConverter* converter = new ImageConverter(this);

    // отображение прогресса обработки
    connect(converter, SIGNAL(needProgressChange(int)),
            this, SLOT(setProgress(int)));

    // конвертирование изображения
//    bool convRes = converter->convertImages(srcImage, dstImage);
    bool convRes = converter->convertWithInterpolation(srcImage, dstImage);
    if (!convRes)
    {
        this->statusBar()->showMessage("Ошибка конвертации изображения", statBarTout);
        return;
    }

    // масштабирование больших изображений
    QSize imageSize = QSize(dstImage->width(), dstImage->height());
    QSize labelSize = ui->lblDstImage->size();

    if (imageSize.height() > labelSize.height() || imageSize.width() > labelSize.width())
        imageSize.scale((labelSize), Qt::KeepAspectRatio);

    // вывод результатов конвертирования
    ui->lblDstImage->setPixmap(QPixmap::fromImage(*dstImage));
    ui->lblDstImage->setScaledContents(true);
    this->statusBar()->showMessage("Конвертация изображения завершена", statBarTout);
}

void ConvertMainWindow::on_bGPU_clicked()
{
    OpenGLWindow *openGLWin = new OpenGLWindow(srcImage, this);
    openGLWin->show();
}

void ConvertMainWindow::setProgress(int val)
{
    // отображение прогресса конвертации
    ui->progressBar->setValue(val);
}


