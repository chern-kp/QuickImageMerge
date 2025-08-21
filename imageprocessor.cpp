#include "imageprocessor.h"
#include <QPainter>
#include <QDebug>

ImageProcessor::ImageProcessor()
{
}

QImage ImageProcessor::stitchImagesVertically(const QStringList &imagePaths, const QColor &backgroundColor)
{
    // A list to hold all successfully loaded images
    QList<QImage> loadedImages;

    // 1. Load all images and find canvas dimensions
    int totalHeight = 0;
    int maxWidth = 0;

    for (const QString &path : imagePaths) {
        QImage image(path);
        if (image.isNull()) {
            qDebug() << "Warning: Could not load image at path:" << path;
            continue; // Skip invalid images
        }
        loadedImages.append(image);
        totalHeight += image.height();
        if (image.width() > maxWidth) {
            maxWidth = image.width();
        }
    }

    if (loadedImages.isEmpty()) {
        qDebug() << "Error: No valid images to stitch.";
        return QImage(); // Return an empty image if nothing was loaded
    }

    // 2. Create the final canvas
    QImage resultImage(maxWidth, totalHeight, QImage::Format_ARGB32_Premultiplied);
    resultImage.fill(backgroundColor); // Fill with black (or any chosen color)

    // 3. Paint each image onto the canvas
    QPainter painter(&resultImage);
    int currentY = 0; // Y-coordinate for the top of the next image

    for (const QImage &image : loadedImages) {
        // As requested, align all images to the left (x = 0)
        painter.drawImage(0, currentY, image);
        currentY += image.height(); // Move down for the next image
    }

    return resultImage;
}