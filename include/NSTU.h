#ifndef NSTR_H_
#define NSTR_H_

// Standard C++.
#include <iostream>
#include <fstream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Tesseract with Leptonica.
#include <tesseract/baseapi.h>
//#include <leptonica/allheaders.h>

// Project headers.
#include "settings.h"
#include "TextDetection/TextDetector.h"
#include "Tagging/Tagger.h"

namespace  NSTU
{
    // Open an image file and extract text from it.
    void processImage(const std::string& inputFileName, const std::string& outputFilePath);

    // Open a video file and extract text from it.
    void processVideo(const std::string& inputFileName, const std::string& outputFilePath);

    // TEST: open a camera stream and extract text from it.
    void processCamera(const std::string& outputFilePath);
}

#endif
