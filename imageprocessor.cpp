#include "imageprocessor.h"
#include <QPainter>
#include <QDebug>
#include <algorithm>

ImageProcessor::ImageProcessor() {}

QImage ImageProcessor::stitchImages(const QStringList &imagePaths,
                                    Orientation orientation,
                                    Alignment alignment,
                                    const QColor &backgroundColor)
{
    QList<QImage> loadedImages;
    if (imagePaths.isEmpty()) return QImage();

    // 1. Load images and calculate final canvas size
    int totalWidth = 0;
    int totalHeight = 0;
    int maxWidth = 0;
    int maxHeight = 0;

    for (const QString &path : imagePaths) {
        QImage image(path);
        if (image.isNull()) {
            qDebug() << "Warning: Could not load image:" << path;
            continue;
        }
        loadedImages.append(image);

        if (orientation == Orientation::Vertical) {
            totalHeight += image.height();
            maxWidth = std::max(maxWidth, image.width());
        } else { // Horizontal
            totalWidth += image.width();
            maxHeight = std::max(maxHeight, image.height());
        }
    }

    if (loadedImages.isEmpty()) return QImage();

    // Final canvas dimensions
    int finalWidth = (orientation == Orientation::Vertical) ? maxWidth : totalWidth;
    int finalHeight = (orientation == Orientation::Vertical) ? totalHeight : maxHeight;

    // 2. Create the final canvas and fill it
    QImage resultImage(finalWidth, finalHeight, QImage::Format_ARGB32_Premultiplied);
    resultImage.fill(backgroundColor);

    // 3. Paint each image onto the canvas with correct alignment
    QPainter painter(&resultImage);
    int currentPos = 0; // currentY for Vertical, currentX for Horizontal

    for (const QImage &image : loadedImages) {
        int x = 0, y = 0;
        if (orientation == Orientation::Vertical) {
            y = currentPos;
            switch (alignment) {
                case Alignment::Left_Top:      x = 0; break;
                case Alignment::Center:        x = (finalWidth - image.width()) / 2; break;
                case Alignment::Right_Bottom:  x = finalWidth - image.width(); break;
            }
            currentPos += image.height();
        } else { // Horizontal
            x = currentPos;
            switch (alignment) {
                case Alignment::Left_Top:      y = 0; break;
                case Alignment::Center:        y = (finalHeight - image.height()) / 2; break;
                case Alignment::Right_Bottom:  y = finalHeight - image.height(); break;
            }
            currentPos += image.width();
        }
        painter.drawImage(x, y, image);
    }

    return resultImage;
}