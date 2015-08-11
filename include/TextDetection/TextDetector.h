#ifndef TEXT_DETECTOR_H_
#define TEXT_DETECTOR_H_

// Standard C++.
#include <iostream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"

namespace NSTU
{
    namespace TextDetection
    {
        class TextDetector
        {
        public:

            // Algorithm selector.
            /*static*/ enum Algorithm { FabioBanfi, TiRG, AndrewPerraultAndSauravKumar };

            // Detect text in an image using a specified algorithm.
            std::vector<cv::Rect> getTextRegions(const cv::Mat& image, Algorithm algorithm) const;

        private:

            // TiRG wrapper.
            std::vector<cv::Rect> TiRGwrapper(const cv::Mat& image) const;

            // Andrew Perrault and Saurav Kumar wrapper.
            std::vector<cv::Rect> AndrewPerraultAndSauravKumarWrapper(const cv::Mat& image) const;
        };
    }
}

#endif