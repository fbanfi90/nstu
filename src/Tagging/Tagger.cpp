#include "Tagging/Tagger.h"

// Standard C++.
#include <iostream>
#include <fstream>

// Project headers.
#include "Tagging/Header.h"
#include "Tagging/Tag.h"

namespace NSTU {
namespace Tagging {

// Tagger constructor ('.xml' for XML, '.yml', '.yaml' or '.txt' for YAML).
Tagger::Tagger(const std::string& inputFilePath, const std::string& outputFilePath) : fileStorage(new cv::FileStorage(outputFilePath, cv::FileStorage::WRITE))
{
    // Check whether file was successfully opened.
    if (!fileStorage->isOpened())
        throw "Cannot open output file.";

    // Remove directory from inputFilePath if present.
    std::string fileName(inputFilePath);
    const auto lastSlashIndex = fileName.find_last_of("\\/");
    if (std::string::npos != lastSlashIndex)
        fileName.erase(0, lastSlashIndex + 1);

    // DEBUG: print header to console.
#ifdef NSTU_DEBUG
    std::cout << Header(fileName, DateTime::now(), Location::here()) << std::endl;
#endif

    // Write file header.
    *fileStorage << "NSTU" << Header(fileName, DateTime::now(), Location::here());
}

// Deconstructor.
Tagger::~Tagger()
{
    // Close the file storage stream;
    fileStorage->release();
}

// Add a text tag.
void Tagger::addTextTag(const std::string& text, const cv::Rect& region, int frameCount)
{
    // Get tag.
    Tag tag(text, region, frameCount);

    // DEBUG: print output line to console.
#ifdef NSTU_DEBUG
    std::cout << tag << std::endl;
#endif

    // Append to output.
    *fileStorage << "Tag" << tag;
}

} // namespace Tagging
} // namespace NSTU