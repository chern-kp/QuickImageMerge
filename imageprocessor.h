#pragma once

#include <QImage>
#include <QStringList>
#include <QColor>

enum class Orientation {
    Vertical,
    Horizontal
};

enum class Alignment {
    Left_Top,
    Center,
    Right_Bottom
};


class ImageProcessor
{
public:
    ImageProcessor();

    //FUNC - Stitch multiple images together
    QImage stitchImages(const QStringList &imagePaths,
                        Orientation orientation,
                        Alignment alignment,
                        const QColor &backgroundColor);
};