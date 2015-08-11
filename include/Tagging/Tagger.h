#ifndef TAGGER_H_
#define TAGGER_H_

// Standard C++.
#include <iostream>
#include <fstream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"

namespace  NSTU
{
    class Tagger
    {
    public:
        
        // Tagger constructor ('.xml' for XML, '.yaml', '.yml' or '.txt' for YAML).
        explicit Tagger(const std::string& inputFilePath, const std::string& outputFilePath);

        // Deconstructor.
        ~Tagger();

        // Add a text tag.
        void addTextTag(const std::string& text, const cv::Rect& region, int frameCount);

    private:

        // File storage stream;
        cv::FileStorage* fileStorage;
    };
}

#endif