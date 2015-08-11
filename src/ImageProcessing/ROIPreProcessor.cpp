#include "ImageProcessing/ROIPreProcessor.h"

// OpenCV.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace  NSTU
{
    namespace ImageProcessing
    {
        /* IMAGE PROCESSING */

        // Pre-process image before applying OCR. The input image is overwritten.
        void ROIPreProcessor::PreProcessImageROI(cv::Mat& image)
        {
            // Convert image to gray.
            cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

            // Invert image colors so that the background is black.
            cv::bitwise_not(image, image);

            // Determine text angle and modify the image so that it is de-skewed.
            deskew(image, getSkewAngle(image));

            // Crop the image around the text.
            //crop(img);

            // Apply some filtering to make the image black and white.
            cv::bitwise_not(image, image);
            cv::GaussianBlur(image, image, cv::Size(3, 3), 0);
            cv::adaptiveThreshold(image, image, 255., cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 75, 10.);
            cv::bitwise_not(image, image);
        }

        // Get the skew angle of text inside an image.
        double ROIPreProcessor::getSkewAngle(const cv::Mat& image) const
        {
            // Perform the Polar Hough Transform on the image and get the corresponding end points.
            auto size = image.size();
            std::vector<cv::Vec4i> lines;
            cv::HoughLinesP(image, lines, 1., CV_PI / 180., 100, size.width / 2., 20.);

            // Calculate the average angle between each line and the horizontal line.
            double angle = 0;
            for (auto line : lines)
                angle += atan2(static_cast<double>(line[3] - line[1]), static_cast<double>(line[2] - line[0]));
            angle /= static_cast<double>(lines.size());

            // Convert the angle from radians to degrees.
            return angle * 180. / CV_PI;
        }

        // Get the minimal rotated rectangle which includeas all non-zero points.
        cv::RotatedRect ROIPreProcessor::getRotatedRectHull(const cv::Mat& image) const
        {
            // Collect all the non-zero points of the image.
            std::vector<cv::Point> points;
            for (auto it = image.begin<uchar>(); it != image.end<uchar>(); ++it)
                if (*it)
                    points.push_back(it.pos());

            // Get the minimal rotated rectangle which includeas all such points.
            return cv::minAreaRect(cv::Mat(points));
        }

        // Deskew an image by a given angle.
        void ROIPreProcessor::deskew(cv::Mat& image, double angle) const
        {
            // Get the text box area in order to determine the center of rotation.
            auto box = getRotatedRectHull(image);

            // Perform rotation using bicubic interpolation and overwrite input image.
            cv::warpAffine(image, image, cv::getRotationMatrix2D(box.center, angle, 1.), image.size(), cv::INTER_CUBIC);
        }

        // Crop ROI of an image.
        void ROIPreProcessor::crop(cv::Mat& image) const
        {
            // Get the quasi-rotated rectangle area around the text.
            auto box = getRotatedRectHull(image);

            // Get the minimal rectangle around the quasi-rotated rectangle.
            image = cv::Mat(image, box.boundingRect());
        }
    }
}