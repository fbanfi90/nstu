#ifndef ROI_PRE_PROCESSOR_H_
#define ROI_PRE_PROCESSOR_H_

// Definition of 'cv::Mat'.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"

namespace  NSTU
{
    namespace ImageProcessing
    {
        class ROIPreProcessor
        {
        public:

            /* IMAGE PROCESSING */

            // Pre-process image before applying OCR.
            void PreProcessImageROI(cv::Mat& image);

            /* VIDEO PROCESSING */

            // Pre-process video frame before applying OCR.
            void PreProcessVideoROI(cv::Mat& frame);

        private:

            /* IMAGE PROCESSING (images assumed to be in grayscale, black interpreted as background) */

            // Get the skew angle of text inside an image.
            double getSkewAngle(const cv::Mat& image) const;

            // Get the minimal rotated rectangle which includeas all non-zero points.
            cv::RotatedRect getRotatedRectHull(const cv::Mat& image) const;

            // Deskew an image by a given angle.
            void deskew(cv::Mat& image, double angle) const;

            // Crop ROI of an image.
            void crop(cv::Mat& image) const;
        };
    }
}

#endif