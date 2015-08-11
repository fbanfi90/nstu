#include "TextDetection/TextDetector.h"

// OpenCV.
//#include <opencv2/highgui/highgui.hpp> // C++
#include <opencv/highgui.h> // C

// Project headers.
#include "TextDetection/FabioBanfi.h" // Fabio Banfi implementation.
#include "TextDetection/trg.hpp" // TiRG.
#include "TextDetection/AndrewPerraultAndSauravKumar.h" // Andrew Perrault and Saurav Kumar implementation.

namespace NSTU
{
    namespace TextDetection
    {
        // Detect text in an image using a specified algorithm.
        std::vector<cv::Rect> TextDetector::getTextRegions(const cv::Mat& image, Algorithm algorithm) const
        {
            switch (algorithm)
            {
            default:
            case Algorithm::FabioBanfi:
                return FabioBanfi::getTextRegionsFB(image);
                break;

            case Algorithm::TiRG:
                return TiRGwrapper(image);
                break;

            case Algorithm::AndrewPerraultAndSauravKumar:
                return AndrewPerraultAndSauravKumarWrapper(image);
                break;
            }
        }

        // TiRG wrapper.
        std::vector<cv::Rect> TextDetector::TiRGwrapper(const cv::Mat& image) const
        {
            // Get image dimensions.
            size_t w = image.size().width;
            size_t h = image.size().height;

            // Create TiRG input vector of vectors.
            std::vector<std::vector<trg::Rgb>> trgImage(h);
            for (size_t i = 0; i < h; ++i)
                trgImage[i] = std::vector<trg::Rgb>(w);

            // Convert image matrix to TiRG vector of vectors input.
            for (size_t y = 0; y < h; ++y)
            {
                for (size_t x = 0; x < w; ++x)
                {
                    size_t pos = 3 * y * w + 3 * x;
                    trg::Rgb pixel = { image.data[pos], image.data[pos + 1], image.data[pos + 2] };
                    trgImage[y][x] = pixel;
                }
            }

            // Get result and convert rectangles from TiRG format to OpenCV format.
            std::vector<cv::Rect> result;
            auto regions = trg::get_textlike_regions(trgImage);
            for (auto region : regions)
                result.push_back(cv::Rect(region.x1, region.y1, region.x2 - region.x1 + 1, region.y2 - region.y1 + 1));
            return result;
        }

        // Andrew Perrault and Saurav Kumar wrapper.
        std::vector<cv::Rect> TextDetector::AndrewPerraultAndSauravKumarWrapper(const cv::Mat& image) const
        {
            IplImage copy = image;
            IplImage* iplImage = &copy;

            // Detect text in the image
            IplImage* output = AndrewPerraultAndSauravKumar::textDetection(iplImage, true);
            //cvReleaseImage(&iplImage);
            cvShowImage("Andrew Perrault and Saurav Kumar", output);
            //cvReleaseImage(&output);

            // Return the detected text regions.
            return std::vector<cv::Rect>();
        }
    }
}