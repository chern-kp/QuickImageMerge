#pragma once

#include <QImage>
#include <QStringList>
#include <QColor>

class ImageProcessor
{
public:
    ImageProcessor();

    QImage stitchImagesVertically(const QStringList &imagePaths, const QColor &backgroundColor);
};