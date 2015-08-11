#ifndef TAG_H_
#define TAG_H_

// Definition of 'cv::Mat'.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"
#include "ISerializable.h"

namespace NSTU
{
    class Tag : ISerializable
    {
    public:

        // Default constructor.
        Tag();

        // Tag constructor.
        Tag(std::string text, cv::Rect region, int frameCount);

        // Write serialization.
        void write(cv::FileStorage& fileStorage) const;

        // Read serialization.
        void read(const cv::FileNode& fileNode);

        // Out stream operator.
        friend std::ostream& operator<<(std::ostream& stream, const Tag& tag);

    public:

        // The detected text.
        std::string text;

        // The region relatice to the frame of the detected text.
        cv::Rect region;

        // Frame counter.
        int frameCount;
    };

    // Must be defined for the serialization in FileStorage to work.
    static void write(cv::FileStorage& fileStorage, const std::string&, const Tag& tag)
    {
        tag.write(fileStorage);
    }

    // Must be defined for the serialization in FileStorage to work.
    static void read(const cv::FileNode& fileNode, Tag& tag, const Tag& defaultValue = Tag())
    {
        if (fileNode.empty())
            tag = defaultValue;
        else
            tag.read(fileNode);
    }
}

#endif