#ifndef UTILS_H_
#define UTILS_H_

// Standard C++.
#include <iostream>

// OpenCV.
#include <opencv2/core/core.hpp>

// Project headers.
#include "../settings.h"

namespace  NSTU
{
    namespace Utils
    {
        /* Utils */

        // Save a cam stream to file.
        void registerCam(std::string fileName);

        /* SVD */

        // Perform a low rank approximation of grayscale image using SVD.
        cv::Mat lowRankApproxG(const cv::Mat&, int rank);

        // Perform a low rank approximation of color image using SVD.
        cv::Mat lowRankApproxC(const cv::Mat&, int rank);

        // TEST: show a low rank approximation of grayscale image using SVD.
        void test_lowRankApproxG(std::string fileName, int rank);

        // TEST: show a low rank approximation of color image using SVD.
        void test_lowRankApproxC(std::string fileName, int rank);

        // TEST: demonstration of low rank grayscale image approximation using SVD.
        void test_lowRankApproxG(std::string fileName);

        // TEST: demonstration of low rank color image approximation using SVD.
        void test_lowRankApproxC(std::string fileName);

        // TEST: demonstration of grayscale image reconstruction using SVD.
        void test_svdG(std::string fileName);

        // TEST: demonstration of color image reconstruction using SVD.
        void test_svdC(std::string fileName);

        // TEST: split camera stream into RGB channels.
        void test_rgbCam();
    };
}

#endif