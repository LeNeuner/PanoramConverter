#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QObject>

// типы сессий
enum class CubeFaceType : unsigned int
{
    Front,
    Back,
    Left,
    Right,
    Top,
    Bottom
};

// хранение точки на кубе
typedef struct CubePoint
{
    double x;
    double y;
    double z;
    CubeFaceType type;
} CubePoint;

class ImageConverter : public QObject
{
    Q_OBJECT
public:
    explicit ImageConverter(const QImage* srcImage, QImage* dstImage,
                            QObject *parent = nullptr);

signals:
    void needProgressChange(int val);

private:
    const QImage *mSrcImage = nullptr;
    QImage       *mDstImage = nullptr;

    bool projectSwitch(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    bool projectToPlane(const double* xyz, const CubeFaceType cubeType, const int cubeSize, int* const xy);

    void projectToFront(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToLeft(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToBack(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToRight(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToTop(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToBottom(const double theta, const double phi, double* const xyz, CubeFaceType &type);
};

#endif // IMAGECONVERTER_H
