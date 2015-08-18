#pragma once

// Standard C++.
#include <vector>

// Project headers.
#include "../settings.h"
#include "ValuePoint.hpp"

namespace NSTU {
namespace TextDetection {
namespace FabioBanfi {

struct Ray
{
    ValuePoint p;
    ValuePoint q;
    std::vector<ValuePoint> points;
};

} // namespace FabioBanfi
} // namespace TextDetection
} // namespace NSTU