#pragma once

// Standard C++.
#include <vector>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"
#include "ValuePoint.hpp"

namespace NSTU {
namespace TextDetection {
namespace FabioBanfi {

// Fabio Banfi's SWT implementation.
std::vector<cv::Rect> getTextRegionsFB(const cv::Mat& image);

// Stroke width transform (SWT).
cv::Mat SWT(const cv::Mat& image, bool darkOnLight, float precision);

// Associate pixels to form connected components.
std::vector<std::vector<ValuePoint>> associate(const cv::Mat& swt);

} // namespace FabioBanfi
} // namespace TextDetection
} // namespace NSTU