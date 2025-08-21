#include "imageprocessor.h"

#include <QPainter>
#include <QDebug>
#include <algorithm>

namespace
{
    // FUNC - Helper function to load all images from paths. It skips images that fail to load and logs a warning.
    QList<QImage> loadImages(const QStringList &imagePaths)
    {
        QList<QImage> loadedImages;
        for (const QString &path : imagePaths)
        {
            QImage image(path);
            if (image.isNull())
            {
                qWarning() << "Could not load image:" << path;
                continue;
            }
            loadedImages.append(image);
        }
        return loadedImages;
    }

    // FUNC - Calculate the final canvas dimensions based on the loaded images and the desired orientation.
    QSize calculateCanvasSize(const QList<QImage> &images, Orientation orientation)
    {
        int totalWidth = 0;
        int totalHeight = 0;
        int maxWidth = 0;
        int maxHeight = 0;

        for (const QImage &image : images)
        {
            if (orientation == Orientation::Vertical)
            {
                totalHeight += image.height();
                maxWidth = std::max(maxWidth, image.width());
            }
            else
            { // Horizontal
                totalWidth += image.width();
                maxHeight = std::max(maxHeight, image.height());
            }
        }

        if (orientation == Orientation::Vertical)
        {
            return {maxWidth, totalHeight};
        }
        else
        {
            return {totalWidth, maxHeight};
        }
    }
}

// FUNC - Stitch multiple images together
QImage ImageProcessor::stitchImages(const QStringList &imagePaths,
                                    Orientation orientation,
                                    CrossAxisAlignment alignment,
                                    const QColor &backgroundColor)
{
    if (imagePaths.isEmpty())
    {
        return QImage();
    }

    // 1. Load images from disk.
    QList<QImage> loadedImages = loadImages(imagePaths);
    if (loadedImages.isEmpty())
    {
        return QImage();
    }

    // 2. Calculate final canvas dimensions.
    const QSize canvasSize = calculateCanvasSize(loadedImages, orientation);
    if (!canvasSize.isValid())
    {
        return QImage();
    }

    // 3. Create the final canvas and fill it with the background color.
    QImage resultImage(canvasSize, QImage::Format_ARGB32_Premultiplied);
    resultImage.fill(backgroundColor);

    // 4. Paint each image onto the canvas with the correct alignment.
    QPainter painter(&resultImage);
    int currentOffset = 0; // currentY for Vertical, currentX for Horizontal

    for (const QImage &image : loadedImages)
    {
        int x = 0;
        int y = 0;

        if (orientation == Orientation::Vertical)
        {
            y = currentOffset;
            switch (alignment)
            {
            case CrossAxisAlignment::Start:
                x = 0;
                break;
            case CrossAxisAlignment::Center:
                x = (canvasSize.width() - image.width()) / 2;
                break;
            case CrossAxisAlignment::End:
                x = canvasSize.width() - image.width();
                break;
            }
            currentOffset += image.height();
        }
        else
        { // Horizontal
            x = currentOffset;
            switch (alignment)
            {
            case CrossAxisAlignment::Start:
                y = 0;
                break;
            case CrossAxisAlignment::Center:
                y = (canvasSize.height() - image.height()) / 2;
                break;
            case CrossAxisAlignment::End:
                y = canvasSize.height() - image.height();
                break;
            }
            currentOffset += image.width();
        }
        painter.drawImage(x, y, image);
    }

    return resultImage;
}