#include "imageconverter.h"

#include <QPainter>
#include <cmath>

#define MAX_TOP_RAD     0.615
#define MIN_BOTTOM_RAD  2.527

ImageConverter::ImageConverter(const QImage *srcImage, QImage *dstImage, QObject *parent)
    : QObject(parent)
{
    // установка изображений для обработки
    mSrcImage = srcImage;
    mDstImage = dstImage;

    // результат обработки
    dstImage = mDstImage;

    // обработка
    int srcWidth  = srcImage->width();
    int srcHeight = srcImage->height();
    int dstWidth  = dstImage->width();
    int dstHeight = dstImage->height();
    int cubeSize  = static_cast<int>(srcWidth / 4);
    double xyz[3] = {0.0, 0.0, 0.0};
//    CubePoint point;
    int xy[2]     = {0, 0};
    CubeFaceType type;
    emit needProgressChange(0);
    for (int x = 0; x < srcWidth; ++x)
    {
        for (int y = 0; y < srcHeight; ++y)
        {
            auto phi   = x * 2 * M_PI / srcWidth;
            auto theta = y * M_PI / srcHeight;

            if (!projectSwitch(theta, phi, xyz, type))
                break;

            if (!projectToPlane(xyz, type, cubeSize, xy))
                break;

            if (xy[0] > dstWidth)
                xy[0] = dstWidth - 1;
            if (xy[1] > dstHeight)
                xy[1] = dstHeight - 1;

            auto pix = srcImage->pixel(x, y);
            dstImage->setPixel(xy[0], xy[1], pix);
            // dstImage->setPixel(x, srcHeight - y - 1, pix);
        }
        int percent = static_cast<int>((x * 100) / srcWidth + 1);
        emit needProgressChange(percent);
    }
}


// проецирование сферических координат на куб
// тета в диапазоне [0; pi], фи в диапазоне [0, 2pi]
bool ImageConverter::projectSwitch(const double theta, const double phi,
                                   double* const xyz, CubeFaceType &type)
{
    if (theta < MAX_TOP_RAD)
        projectToTop(theta, phi, xyz, type);
    else if (theta > MIN_BOTTOM_RAD)
        projectToBottom(theta, phi, xyz, type);
    else if ((phi <= M_PI_4) || (phi > 7 * M_PI_4))
        projectToLeft(theta, phi, xyz, type);
    else if ((phi > M_PI_4) && (phi <= 3 * M_PI_4))
        projectToFront(theta, phi, xyz, type);
    else if ((phi > 3 * M_PI_4) && (phi <= 5 * M_PI_4))
        projectToRight(theta, phi, xyz, type);
    else if ((phi > 5 * M_PI_4) && (phi <= 7 * M_PI_4))
        projectToBack(theta, phi, xyz, type);
    else
        return false;
    return true;
}

// проецирование кубических координат на развертку
bool ImageConverter::projectToPlane(const double *xyz, const CubeFaceType cubeType,
                                    const int cubeSize, int * const xy)
{
    if (cubeType == CubeFaceType::Front)
    {
        xy[0] = static_cast<int>(cubeSize * (xyz[0] + 3) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Left)
    {
        xy[0] = static_cast<int>(cubeSize * (xyz[1] + 1) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Right)
    {
        xy[0] = static_cast<int>(cubeSize * (5 - xyz[1]) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Back)
    {
        xy[0] = static_cast<int>(cubeSize * (7 - xyz[0]) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Top)
    {
        xy[0] = static_cast<int>(cubeSize * (3 - xyz[0]) / 2);
        xy[1] = static_cast<int>(cubeSize * (1 + xyz[1]) / 2);
    }
    else if (cubeType == CubeFaceType::Bottom)
    {
        xy[0] = static_cast<int>(cubeSize * (3 - xyz[0]) / 2);
        xy[1] = static_cast<int>(cubeSize * (5 - xyz[1]) / 2);
    }
    else
        return false;
    return true;
}

// проецирование на отдельные грани
void ImageConverter::projectToFront(const double theta, const double phi,
                                    double * const xyz, CubeFaceType &type)
{
    xyz[0] = tan(phi - M_PI_2);
    xyz[1] = 1;
    xyz[2] = 1 / (tan(theta) * cos(phi - M_PI_2));
    type = CubeFaceType::Front;

    if (xyz[2] < -1)
    {
        projectToBottom(theta, phi, xyz, type);
        return;
    }
    if (xyz[2] > 1)
    {
        projectToTop(theta, phi, xyz, type);
        return;
    }
}
void ImageConverter::projectToLeft(const double theta, const double phi,
                                   double * const xyz, CubeFaceType &type)
{
    xyz[0] = 1;
    xyz[1] = tan(phi);
    xyz[2] = 1 / (tan(theta) * cos(phi));
    type = CubeFaceType::Left;

    if (xyz[2] < -1)
    {
        projectToBottom(theta, phi, xyz, type);
        return;
    }
    if (xyz[2] > 1)
    {
        projectToTop(theta, phi, xyz, type);
        return;
    }
}
void ImageConverter::projectToBack(const double theta, const double phi,
                                   double * const xyz, CubeFaceType &type)
{
    xyz[0] = tan(phi - 3 * M_PI_2);
    xyz[1] = -1;
    xyz[2] = 1 / (tan(theta) * cos(phi - 3 * M_PI_2));
    type = CubeFaceType::Back;

    if (xyz[2] < -1)
    {
        projectToBottom(theta, phi, xyz, type);
        return;
    }
    if (xyz[2] > 1)
    {
        projectToTop(theta, phi, xyz, type);
        return;
    }
    xyz[0] = -xyz[0];
}
void ImageConverter::projectToRight(const double theta, const double phi,
                                    double * const xyz, CubeFaceType &type)
{
    xyz[0] = -1;
    xyz[1] = tan(phi);
    xyz[2] = -1 / (tan(theta) * cos(phi));
    type = CubeFaceType::Right;

    if (xyz[2] < -1)
    {
        projectToBottom(theta, phi, xyz, type);
        return;
    }
    if (xyz[2] > 1)
    {
        projectToTop(theta, phi, xyz, type);
        return;
    }
    xyz[1] = -xyz[1];
}
void ImageConverter::projectToTop(const double theta, const double phi,
                                  double * const xyz, CubeFaceType &type)
{
    // a sinθ cosø, a sinθ sinø, a cosθ) = (x, y, 1)
    double a = 1 / cos(theta);
    xyz[0] = tan(theta) * cos(phi);
    xyz[1] = tan(theta) * sin(phi);
    xyz[2] = 1;
    type = CubeFaceType::Top;
}
void ImageConverter::projectToBottom(const double theta, const double phi,
                                     double * const xyz, CubeFaceType &type)
{
    // a sinθ cosø, a sinθ sinø, a cosθ) = (x, y, -1)
    double a = -1 / cos(theta);
    xyz[0] = -tan(theta) * cos(phi);
    xyz[1] = -tan(theta) * sin(phi);
    xyz[2] = -1;
    type = CubeFaceType::Bottom;
}


