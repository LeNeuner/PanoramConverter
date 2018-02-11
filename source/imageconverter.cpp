#include "imageconverter.h"

#include <QPainter>
#include <cmath>

#define MAX_TOP_RAD     0.615
#define MIN_BOTTOM_RAD  2.527

template <typename T>
static inline T clamp(const T &val, const T &lower, const T &upper)
{
    return std::min(std::max(val, lower), upper);
}

ImageConverter::ImageConverter(QObject *parent)
    : QObject(parent)
{
}

// основная функция конвертирования изображений (без интерполяции)
bool ImageConverter::convertImages(const QImage *srcImage, QImage *dstImage)
{
    // подготовка исходных данных
    int srcWidth  = srcImage->width();
    int srcHeight = srcImage->height();
    int dstWidth  = dstImage->width();
    int dstHeight = dstImage->height();
    int cubeSize  = static_cast<int>(srcWidth / 4);
    double xyz[3] = {0.0, 0.0, 0.0};
    int xy[2]     = {0, 0};
    CubeFaceType type;

    // конвертирование
    emit needProgressChange(0);
    for (int x = 0; x < srcWidth; ++x)
    {
        for (int y = 0; y < srcHeight; ++y)
        {
            // определение полярных координат
            auto phi   = x * 2 * M_PI / srcWidth;
            auto theta = y * M_PI / srcHeight;

            // проецирование полярных координат на куб
            if (!projectSwitch(theta, phi, xyz, type))
                return false;

            // проецирование кубических координат на развертку
            if (!projectToPlane(xyz, type, cubeSize, xy))
                return false;

            // обработка превышений размеров
            if (xy[0] > dstWidth)
                xy[0] = dstWidth - 1;
            if (xy[1] > dstHeight)
                xy[1] = dstHeight - 1;

            // работа с интенсивностью
            auto pix = srcImage->pixel(x, y);
            dstImage->setPixel(xy[0], xy[1], pix);
        }
        int percent = static_cast<int>((x * 100) / srcWidth + 1);
        emit needProgressChange(percent);
    }
    return true;
}

// основная функция конвертирования изображений (с интерполяцией)
bool ImageConverter::convertWithInterpolation(const QImage *srcImage, QImage *dstImage)
{
    // подготовка исходных данных
    int srcWidth  = srcImage->width();
    int srcHeight = srcImage->height();
    int dstWidth  = dstImage->width();
    int cubeSize  = static_cast<int>(srcWidth / 4);
    double xyz[3] = {0.0, 0.0, 0.0};
    int xy[2]     = {0, 0};
    CubeFaceType type;

    // конвертирование
    for (int x = 0; x < dstWidth; ++x)
    {
        // определение текущего квадранта по оси X
        int quadVal = static_cast<int>(x / cubeSize);
        int startY, stopY;
        if (quadVal == 1)
        {
            startY = 0;
            stopY  = 3 * cubeSize;
        }
        else
        {
            startY = cubeSize;
            stopY  = 2 * cubeSize;
        }
        for (int y = startY; y < stopY; ++y)
        {
            // определение текущей грани куба
            int faceVal;
            if (y < cubeSize)
                faceVal = 4;
            else if (y >= 2 * cubeSize)
                faceVal = 5;
            else
                faceVal = quadVal;

            // получение типа грани
            type = faceValToCubeType(faceVal);

            // перевод развертки в кубические координаты
            xy[0] = x;
            xy[1] = y;
            if (!projectToCubic(xy, type, cubeSize, xyz))
                return false;

            // проецирование кубических координат в полярные
            auto theta = atan2(xyz[1], xyz[0]);
            auto r     = hypot(xyz[0], xyz[1]);
            auto phi   = atan2(xyz[2], r);

            // координаты на исходом изображении
            auto uDbl  = 2.0 * cubeSize * (theta + M_PI) / M_PI;
            auto vDbl  = 2.0 * cubeSize * (M_PI_2 - phi) / M_PI;

            // формирование недостающих координат
            int uInt   = static_cast<int>(floor(uDbl));
            int vInt   = static_cast<int>(floor(vDbl));
            int uTR    = uInt + 1;
            int vTR    = vInt + 1;
            auto uDif  = uDbl - uInt;
            auto vDif  = vDbl - vInt;

            // работа с интенсивностью
            auto pixBL = srcImage->pixel(uInt % srcWidth, clamp(vInt, 0, srcHeight -1));
            auto pixBR = srcImage->pixel(uTR  % srcWidth, clamp(vInt, 0, srcHeight -1));
            auto pixTL = srcImage->pixel(uInt % srcWidth, clamp(vTR,  0, srcHeight -1));
            auto pixTR = srcImage->pixel(uTR  % srcWidth, clamp(vTR,  0, srcHeight -1));

            auto pixR = qRed(pixBL)   * (1 - uDif) * (1 - vDif) + qRed(pixBR)   * uDif * (1 - vDif) +
                        qRed(pixTL)   * (1 - uDif) * vDif       + qRed(pixTR)   * uDif * vDif;
            auto pixG = qGreen(pixBL) * (1 - uDif) * (1 - vDif) + qGreen(pixBR) * uDif * (1 - vDif) +
                        qGreen(pixTL) * (1 - uDif) * vDif       + qGreen(pixTR) * uDif * vDif;
            auto pixB = qBlue(pixBL)  * (1 - uDif) * (1 - vDif) + qBlue(pixBR)  * uDif * (1 - vDif) +
                        qBlue(pixTL)  * (1 - uDif) * vDif       + qBlue(pixTR)  * uDif * vDif;

            QRgb pix = qRgb(round(pixR), round(pixG), round(pixB));
            dstImage->setPixel(x, y, pix);
        }
        int percent = static_cast<int>((x * 100) / srcWidth + 1);
        emit needProgressChange(percent);
    }
    return true;
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

// размещение кубических координат на развертке
bool ImageConverter::projectToPlane(const double *xyz, const CubeFaceType cubeType,
                                    const int cubeSize, int * const xy)
{
    if (cubeType == CubeFaceType::Front)
    {
        xy[0] = static_cast<int>(cubeSize * (3 + xyz[0]) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Left)
    {
        xy[0] = static_cast<int>(cubeSize * (1 + xyz[1]) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Back)
    {
        xy[0] = static_cast<int>(cubeSize * (7 - xyz[0]) / 2);
        xy[1] = static_cast<int>(cubeSize * (3 - xyz[2]) / 2);
    }
    else if (cubeType == CubeFaceType::Right)
    {
        xy[0] = static_cast<int>(cubeSize * (5 - xyz[1]) / 2);
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

// перевод развертки в кубические координаты
bool ImageConverter::projectToCubic(const int *xy, const CubeFaceType &cubeType,
                                    const int &cubeSize, double * const xyz)
{
    double coefX = static_cast<double>(2.0 * xy[0] / cubeSize);
    double coefY = static_cast<double>(2.0 * xy[1] / cubeSize);

    if (cubeType == CubeFaceType::Left)
    {
        xyz[0] = coefX - 1.0;
        xyz[1] = -1.0;
        xyz[2] = 3.0 - coefY;
    }
    else if (cubeType == CubeFaceType::Front)
    {
        xyz[0] = 1.0;
        xyz[1] = coefX - 3.0;
        xyz[2] = 3.0 - coefY;
    }
    else if (cubeType == CubeFaceType::Right)
    {
        xyz[0] = 5.0 - coefX;
        xyz[1] = 1.0;
        xyz[2] = 3.0 - coefY;
    }
    else if (cubeType == CubeFaceType::Back)
    {
        xyz[0] = -1.0;
        xyz[1] = 7.0 - coefX;
        xyz[2] = 3.0 - coefY;
    }
    else if (cubeType == CubeFaceType::Top)
    {
        xyz[0] = coefY - 1.0;
        xyz[1] = coefX - 3;
        xyz[2] = 1.0;
    }
    else if (cubeType == CubeFaceType::Bottom)
    {
        xyz[0] = 5.0 - coefY;
        xyz[1] = coefX - 3.0;
        xyz[2] = -1.0;
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
    // a sinθ cosø, a sinθ sinø, a cosθ) = (x, y, 1) : a = 1 / cos(theta)
    xyz[0] = tan(theta) * cos(phi);
    xyz[1] = tan(theta) * sin(phi);
    xyz[2] = 1;
    type = CubeFaceType::Top;
}
void ImageConverter::projectToBottom(const double theta, const double phi,
                                     double * const xyz, CubeFaceType &type)
{
    // a sinθ cosø, a sinθ sinø, a cosθ) = (x, y, -1) : a = -1 / cos(theta)
    xyz[0] = -tan(theta) * cos(phi);
    xyz[1] = -tan(theta) * sin(phi);
    xyz[2] = -1;
    type = CubeFaceType::Bottom;
}

CubeFaceType ImageConverter::faceValToCubeType(const int &faceVal)
{
    CubeFaceType type;
    if (faceVal == 0)
        type = CubeFaceType::Left;
    else if (faceVal == 1)
        type = CubeFaceType::Front;
    else if (faceVal == 2)
        type = CubeFaceType::Right;
    else if (faceVal == 3)
        type = CubeFaceType::Back;
    else if (faceVal == 4)
        type = CubeFaceType::Top;
    else
        type = CubeFaceType::Bottom;
    return type;
}


