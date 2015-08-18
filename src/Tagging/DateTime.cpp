#include "Tagging/DateTime.h"

// Standard C++.
#include <ctime>

// OpenCV.
#include <opencv2/core/core.hpp>

namespace NSTU {
namespace Tagging {

// Default constructor (UNIX epoch).
DateTime::DateTime() : day(1), month(1), year(1970), hour(0), minute(0), second(0)
{
}

// Date and time constructor.
DateTime::DateTime(int day, int month, int year, int hour, int minute, int second) : day(day), month(month), year(year), hour(hour), minute(minute), second(second)
{
}

// The DateTime object representing current date and time.
DateTime DateTime::now()
{
    // Get current time.
    time_t t = time(nullptr);
    tm* now = localtime(&t);

    // Set date and time members.
    return DateTime(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900, now->tm_hour, now->tm_min, now->tm_sec);
}

// Write serialization.
void DateTime::write(cv::FileStorage& fileStorage) const
{
    fileStorage << "{";
    fileStorage << "Day" << day;
    fileStorage << "Month" << month;
    fileStorage << "Year" << year;
    fileStorage << "Hour" << hour;
    fileStorage << "Minute" << minute;
    fileStorage << "Second" << second;
    fileStorage << "}";
}

// Read serialization.
void DateTime::read(const cv::FileNode& fileNode)
{
    day = (int)fileNode["Day"];
    month = (int)fileNode["Month"];
    year = (int)fileNode["Year"];
    hour = (int)fileNode["Hour"];
    minute = (int)fileNode["Minute"];
    second = (int)fileNode["Second"];
}

// Out stream operator.
std::ostream& operator<<(std::ostream& stream, const DateTime& dateTime)
{
    // Create string representation.
    stream << dateTime.day << "/" << dateTime.month << "/" << dateTime.year << "-";
    stream << dateTime.hour << ":" << (dateTime.minute < 10 ? "0" : "") << dateTime.minute << ":" << (dateTime.second < 10 ? "0" : "") << dateTime.second;

    // Return the modified stream.
    return stream;
}

} // namespace Tagging
} // namespace NSTU