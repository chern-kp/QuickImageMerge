#pragma once

#include <QImage>
#include <QStringList>
#include <QColor>

// The direction in which images are stitched together.
enum class Orientation {
    Vertical,
    Horizontal
};

// How images are aligned on the axis perpendicular to the orientation.
// For Vertical orientation: Start=Left, Center=Center, End=Right.
// For Horizontal orientation: Start=Top, Center=Center, End=Bottom.
enum class CrossAxisAlignment {
    Start,
    Center,
    End
};

class ImageProcessor
{
public:
    ImageProcessor() = default;

    //FUNC - Stitch multiple images together
    QImage stitchImages(const QStringList& imagePaths,
                        Orientation orientation,
                        CrossAxisAlignment alignment,
                        const QColor& backgroundColor);
};