#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H

#include <QObject>

class ImageConverter : public QObject
{
    Q_OBJECT
public:
    explicit ImageConverter(QObject *parent = nullptr);
    explicit ImageConverter(const QImage* srcImage, QImage* dstImage, QObject *parent = nullptr);

signals:

public slots:
};

#endif // IMAGECONVERTER_H
