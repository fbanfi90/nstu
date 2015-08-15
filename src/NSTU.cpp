#include "NSTU.h"

// Standard C++.
#include <iostream>
#include <fstream>
#include <string>

// OpenCV.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Tesseract with Leptonica.
#include <tesseract/baseapi.h>
//#include <leptonica/allheaders.h>

// Project headers.
#include "TextDetection/TextDetector.h"
#include "ImageProcessing/ROIPreProcessor.h"
#include "Tagging/Tagger.h"

namespace NSTU
{
    // Process a single image / frame.
    void processMat(const cv::Mat& image, int frameCount, TextDetection::TextDetector& textDetector, tesseract::TessBaseAPI& tessBaseApi, Tagger& tagger)
    {
        // DEBUG: copy image for displaying detected regions.
#ifdef NSTU_DEBUG
        auto copy = image.clone();
#endif

        // Increase image brightness (to be adjusted depending on stream input).
        double alpha = 1.5; // 0-3
        double beta = 50; // 0-100
        image.convertTo(image, -1, alpha, beta);

        // Find all text regions.
        for (auto& region : textDetector.getTextRegions(image, TextDetection::TextDetector::Algorithm::TiRG))
        {
            // Enlargement factor.
            auto d = 10;

            // Enlarge region respecting image's borders.
            region.x = region.x - d > 0 ? region.x - d : 0;
            region.y = region.y - d > 0 ? region.y - d : 0;
            region.width = region.x + region.width + 2 * d < image.cols ? region.width + 2 * d : image.cols - region.x;
            region.height = region.y + region.height + 2 * d < image.rows ? region.height + 2 * d : image.rows - region.y;

            // Create region sub-image.
            auto subImage = image(region);

            /*// Pre-process region before applying OCR.
            ImageProcessing::ROIPreProcessor roiPreProcessor;
            roiPreProcessor.PreProcessRegion(subImage);
            //cv::imshow("Pre-processed ROI", subImage);*/

            // Pass the OpenCV image to Tesseract.
            tessBaseApi.SetImage(subImage.data, subImage.cols, subImage.rows, subImage.channels(), subImage.step1());

            // Try to recognize text in sub-image.
            tessBaseApi.Recognize(0);

            // Get the recognized text.
            auto text = std::string(tessBaseApi.GetUTF8Text());

            // WORKAROUND: ignore string with special characters.
            if (text.length() > 0 && text.find("[") == std::string::npos && text.find("]") == std::string::npos && text.find("{") == std::string::npos && text.find("}") == std::string::npos)
            {
                // Remove trailing newlines.
                text = text.substr(0, text.length() - 2);

                // Tag text region.
                tagger.addTextTag(text, region, frameCount);
            }

            // DEBUG: add a green rectangle and the recognized text for this region in the frame.
#ifdef NSTU_DEBUG
            cv::rectangle(copy, region, cv::Scalar(0., 255., 0.));
            if (text.empty())
                text = " ";
            cv::putText(copy, text, region.tl(), cv::FONT_HERSHEY_PLAIN, 2., cv::Scalar::all(255.));
#endif
        }

        // DEBUG: show the image with the rectangles in a window.
#ifdef NSTU_DEBUG
        cv::imshow("NSTU", copy);
#endif
    }

    // Open an image file and extract text from it.
    void processImage(const std::string& inputFilePath, const std::string& outputFilePath)
    {
        // Open file by default as color image.
        auto image = cv::imread(inputFilePath);

        // Check whether the file was a valid image.
        if (image.data == nullptr)
            throw "Could not open or find image file.";

        // Initialize text detection object.
        TextDetection::TextDetector textDetector;

        // Initialize tesseract.
        tesseract::TessBaseAPI tessBaseApi;
        tessBaseApi.Init("", "eng", tesseract::OEM_DEFAULT);
        tessBaseApi.SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);

        // Initialize tagger object.
        Tagger tagger(inputFilePath, outputFilePath);

        // Process the image.
        processMat(image, 0, textDetector, tessBaseApi, tagger);

        // DEBUG: let show debug image.
#ifdef NSTU_DEBUG
        while (cv::waitKey() != 'q');
#endif
    }

    // Open a video file and extract text from it.
    void processVideo(const std::string& inputFilePath, const std::string& outputFilePath)
    {
        // Try to open the default camera.
        cv::VideoCapture capture(inputFilePath);

        // Check whether could open the default camera.
        if (!capture.isOpened())
            throw "Could not open or find video file.";

        // Initialize text detection object.
        TextDetection::TextDetector textDetector;

        // Initialize tesseract.
        tesseract::TessBaseAPI tessBaseApi;
        tessBaseApi.Init("", "eng", tesseract::OEM_DEFAULT);
        tessBaseApi.SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);

        // Initialize tagger object.
        Tagger tagger(inputFilePath, outputFilePath);

        // Frames counter.
        auto frameCount = 0;

        // Loop forever.
        while (true)
        {
            // Get a new frame from camera.
            cv::Mat frame;
            capture >> frame;

            // The frame must not be empty.
            if (!frame.empty())
            {
                // Increase counter.
                ++frameCount;

                // Process the frame.
                processMat(frame, frameCount, textDetector, tessBaseApi, tagger);

                // Wait until key press.
#ifdef NSTU_DEBUG
                if (cv::waitKey(25) == 'q')
                    break;
#endif
            }

            // Finished.
            else
                break;
        }
    }

    // TEST: open a camera stream and extract text from it.
    void processCamera(const std::string& outputFilePath)
    {
        // Warn that function is available only in debug mode.
#ifndef NSTU_DEBUG
        throw "Compile in debug mode in order to use this function.";
#else

        // Try to open the default camera.
        cv::VideoCapture capture(CV_CAP_ANY);

        // Check whether could open the default camera.
        if (!capture.isOpened())
            throw "Could not open default camera.";

        // Initialize text detection object.
        TextDetection::TextDetector textDetector;

        // Initialize tesseract.
        tesseract::TessBaseAPI tessBaseApi;
        tessBaseApi.Init("", "eng", tesseract::OEM_DEFAULT);
        tessBaseApi.SetPageSegMode(tesseract::PageSegMode::PSM_SINGLE_LINE);

        // Initialize tagger object.
        Tagger tagger("camera", outputFilePath);

        // Frames counter.
        auto frameCount = 0;

        // Loop forever.
        while (true)
        {
            // Get a new frame from camera.
            cv::Mat frame;
            capture >> frame;

            // The frame must not be empty.
            if (!frame.empty())
            {
                // Increase counter.
                ++frameCount;

                // Process the frame.
                processMat(frame, frameCount, textDetector, tessBaseApi, tagger);

                // Wait until key press.
                if (cv::waitKey(25) == 'q')
                    break;
            }
        }
#endif
    }
}
