// image_utils.h

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <QImage>
#include <opencv2/core/mat.hpp>
#include <QDebug>

namespace ImageUtils {

cv::Mat qimage_to_mat(const QImage& img) {
    // Convert image to RGB888 if it isn't already
    QImage converted = img;
    if (img.format() != QImage::Format_RGB888) {
        converted = img.convertToFormat(QImage::Format_RGB888);
    }

    if (converted.isNull()) {
        qDebug() << "Failed to convert image format";
        return cv::Mat();
    }

    return cv::Mat(converted.height(), converted.width(), CV_8UC3,
                   const_cast<uchar*>(converted.bits()),
                   converted.bytesPerLine()).clone();
}

}  // namespace ImageUtils

#endif // IMAGE_UTILS_H
