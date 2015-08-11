#include "Tagging/Tag.h"

// OpenCV.
#include <opencv2/core/core.hpp>

namespace NSTU
{
    // Default constructor.
    Tag::Tag() : text(), region()
    {
    }

    // Constructor.
    Tag::Tag(std::string text, cv::Rect region, int frameCount) : text(text), region(region), frameCount(frameCount)
    {
    }

    // Write serialization.
    void Tag::write(cv::FileStorage& fileStorage) const
    {
        fileStorage << "{";
        fileStorage << "Text" << text;
        fileStorage << "Region" << region;
        fileStorage << "FrameCount" << frameCount;
        fileStorage << "}";
    }

    // Read serialization.
    void Tag::read(const cv::FileNode& fileNode)
    {
        text = (std::string)fileNode["Text"];

        // Workaround becaus cv::Rect hasn't defined read.
        std::vector<int> rect;
        fileNode["Region"] >> rect;
        region = cv::Rect(rect[0], rect[1], rect[2], rect[3]);

        frameCount = (int)fileNode["FrameCount"];
    }

    // Out stream operator.
    std::ostream& operator<<(std::ostream& stream, const Tag& tag)
    {
        // Create string representation.
        stream << "Frame " << tag.frameCount << ", region " << tag.region << ":\t" << tag.text;

        // Return the modified stream.
        return stream;
    }
}

