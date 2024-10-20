// image_utils.h

#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include <QImage>
#include <opencv2/core/mat.hpp>

namespace ImageUtils {

cv::Mat qimage_to_mat(const QImage& img) {
    if (img.format() != QImage::Format_RGB888) {
        return cv::Mat();
    }
    return cv::Mat(img.height(), img.width(), CV_8UC3, const_cast<uchar*>(img.bits()), img.bytesPerLine()).clone();
}

}  // namespace ImageUtils

#endif // IMAGE_UTILS_H
