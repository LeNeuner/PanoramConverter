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

class ImageConverter : public QObject
{
    Q_OBJECT
public:
    explicit ImageConverter(QObject *parent = nullptr);

    /**
     * @brief convertImages Конвертирует входное изображение из
     * эквиректангулярной проекции в кубическую (в развертке)
     * @param srcImage, указатель на входное изображение
     * @param dstImage, указатель на выходное изображение
     * @return true, если конвертация прошла успешно, false иначе
     */
    bool convertImages(const QImage *srcImage, QImage *dstImage);
    /**
     * @brief convertWithInterpolation Конвертирует входное изображение из
     * эквиректангулярной проекции в кубическую (в развертке) c использованием
     * обратного преобразования и интерполяции
     * @param srcImage, указатель на входное изображение
     * @param dstImage, указатель на выходное изображение
     * @return true, если конвертация прошла успешно, false иначе
     */
    bool convertWithInterpolation(const QImage *srcImage, QImage *dstImage);

signals:
    void needProgressChange(int val);

private:
    bool projectSwitch(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    bool projectToPlane(const double* xyz, const CubeFaceType cubeType, const int cubeSize, int* const xy);

    bool projectToCubic(const int *xy, const CubeFaceType &cubeType, const int &cubeSize, double* const xyz);

    void projectToFront(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToLeft(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToBack(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToRight(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToTop(const double theta, const double phi, double* const xyz, CubeFaceType &type);
    void projectToBottom(const double theta, const double phi, double* const xyz, CubeFaceType &type);

    CubeFaceType faceValToCubeType(const int &faceVal);
};

#endif // IMAGECONVERTER_H
