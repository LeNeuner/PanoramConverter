#include "imageconverter.h"

#include <QPainter>
#include <cmath>

ImageConverter::ImageConverter(QObject *parent)
    : QObject(parent)
{

}

ImageConverter::ImageConverter(const QImage *srcImage, QImage *dstImage, QObject *parent)
    : QObject(parent)
{

    // test
    QRgb rgb;
    if ((srcImage != nullptr) && (dstImage != nullptr))
    {
        for (int x = 0; x < srcImage->width(); x++)
        {
            for (int y = 30; y < 70; y++)
            {
                rgb = qRgb(0, 230, 0);
                dstImage->setPixel(x, y, rgb);
            }
        }
    }
}
