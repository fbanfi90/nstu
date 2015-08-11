#include "Tagging/Header.h"

// OpenCV.
#include <opencv2/core/core.hpp>

namespace NSTU
{
    // Default constructor.
    Header::Header() : fileName(), dateTime(), location()
    {
    }

    // Location constructor.
    Header::Header(std::string fileName, DateTime dateTime, Location location) : fileName(fileName), dateTime(dateTime), location(location)
    {
    }

    // Write serialization.
    void Header::write(cv::FileStorage& fileStorage) const
    {
        fileStorage << "{";
        fileStorage << "Version" << VERSION;
        fileStorage << "FileName" << fileName;
        fileStorage << "DateTime" << dateTime;
        fileStorage << "Location" << location;
        fileStorage << "}";
    }

    // Read serialization.
    void Header::read(const cv::FileNode& fileNode)
    {
        fileName = (std::string)fileNode["FileName"];
        dateTime.read(fileNode["DateTime"]);
        location.read(fileNode["Location"]);
    }

    // Out stream operator.
    std::ostream& operator<<(std::ostream& stream, const Header& header)
    {
        // Create string representation.
        stream << "Version: " << VERSION << std::endl;
        stream << "FileName: " << header.fileName << std::endl;
        stream << "DateTime: " << header.dateTime << std::endl;
        stream << "Location: " << header.location;

        // Return the modified stream.
        return stream;
    }
}