#pragma once

// Standard C++.
#include <iostream>
#include <fstream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"

namespace NSTU {
namespace Tagging {

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

} // namespace Tagging
} // namespace NSTU