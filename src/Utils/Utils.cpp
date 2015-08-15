#include "Utils/Utils.h"

// Standard C++.
#include <iostream>
#include <iomanip>

// OpenCV.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace  NSTU
{
    namespace Utils
    {
        /* Utils */

        // Save a cam stream to file.
        void registerCam(std::string fileName)
        {
            // Try to open the default camera.
            cv::VideoCapture capture(CV_CAP_ANY);

            // Check whether could open the default camera.
            if (!capture.isOpened())
                throw "Could not open or find video file.";

            // Acquire input size.
            auto size = cv::Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));

            // The output video stream.
            cv::VideoWriter out(fileName, CV_FOURCC_PROMPT, capture.get(CV_CAP_PROP_FPS), size);

            // Check whether out stream is open.
            if (!out.isOpened())
                throw "Could not find an appropriate codec.";

            // Loop forever.
            while (true)
            {
                // Get a new frame from camera.
                cv::Mat frame;
                capture >> frame;

                // The frame must not be empty.
                if (!frame.empty())
                {
                    // Write to the output video stream.
                    out << frame;

                    // Display frame.
                    cv::imshow("Register", frame);

                    // Wait until key press.
                    if (cv::waitKey(25) == 'q')
                        break;
                }
            }
        }

        /* SVD */

        // Perform a low rank approximation of grey scale image using SVD.
        cv::Mat lowRankApproxG(const cv::Mat& image, int rank)
        {
            // Get the smaller matrix dimension (maximal possible rank).
            auto n = std::min(image.rows, image.cols);

            // Get a grey scale float matrix copy of the image.
            cv::Mat copy;
            cv::cvtColor(image, copy, CV_BGR2GRAY);
            copy.convertTo(copy, CV_32FC1, 1. / 255.);

            // Perform SVD on copy matrix.
            cv::SVD svd(copy, cv::SVD::MODIFY_A);

            // Get singular values matrix Sigma from vector 'svd.w'.
            cv::Mat s(n, n, CV_32FC1);
            for (auto i = 0; i < n; ++i)
                s.at<cv::Vec<float, 1>>(i, i) = svd.w.at<cv::Vec<float, 1>>(i);

            // Return r-rank approximation of the image, that is 'Sum from 1 to k of u_k * s_k * v'_k' = U(:, 1:r) * S(1:r, 1:r) * V'(1:r, :).
            return svd.u(cv::Range::all(), cv::Range(0, rank)) * s(cv::Range(0, rank), cv::Range(0, rank)) * svd.vt(cv::Range(0, rank), cv::Range::all());
        }

        // Perform a low rank approximation of colour image using SVD.
        cv::Mat lowRankApproxC(const cv::Mat& image, int rank)
        {
            throw "Function not implemented yet.";
        }

        // TEST: show a low rank approximation of grey scale image using SVD.
        void test_lowRankApproxG(std::string fileName, int rank)
        {
            // Open file by default as colour image.
            auto image = cv::imread(fileName);

            // Check whether the file was a valid image.
            if (image.data == nullptr)
                throw "Could not open or find image file.";

            // Show the r-rank grey scale approximation of the image.
            cv::imshow("", lowRankApproxG(image, rank));
            while (cv::waitKey() != 'q');
        }

        // TEST: show a low rank approximation of colour image using SVD.
        void test_lowRankApproxC(std::string fileName, int rank)
        {
            // Open file by default as colour image.
            auto image = cv::imread(fileName);

            // Check whether the file was a valid image.
            if (image.data == nullptr)
                throw "Could not open or find image file.";

            // Show the r-rank colour approximation of the image.
            cv::imshow("", lowRankApproxC(image, rank));
            while (cv::waitKey() != 'q');
        }

        // TEST: demonstration of low rank grey scale image approximation using SVD.
        void test_lowRankApproxG(std::string fileName)
        {
            // Open file as grey scale image.
            auto image = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);

            // Check whether the file was a valid image.
            if (image.data == nullptr)
                throw "Could not open or find image file.";

            // Get the smaller matrix dimension (maximal possible rank).
            auto n = std::min(image.rows, image.cols);

            // Transform the image into a float matrix.
            image.convertTo(image, CV_32F, 1. / 255.);

            // Perform SVD on image matrix.
            cv::SVD svd(image, cv::SVD::MODIFY_A);

            // Reset image matrix.
            image = cv::Mat(image.size(), image.type());

            // Perform sum of 1-rank approximation matrices.
            for (auto r = 1; r <= n; ++r)
            {
                // Add a new 1-rank matrix approximation of weight sigma_i to the (i - 1)-rank approximation.
                image += svd.u(cv::Range::all(), cv::Range(r - 1, r)) * svd.w(cv::Range(r - 1, r), cv::Range::all()) * svd.vt(cv::Range(r - 1, r), cv::Range::all());

                // Show the i-rank approximation.
                cv::imshow("LRA", image);

                // Print approximation percentage.
                std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(1) << "\r" << (static_cast<float>(r) / n) * 100. << "%";

                // Print compression percentage.
                std::cout << " (" <<  static_cast<float>(r) * (image.rows + image.cols + 1.) / (image.rows * image.cols) * 100. << "%)";

                // Wait for key press and terminate if was 'q'.
                if (cv::waitKey() == 'q')
                {
                    std::cout << std::endl;
                    return;
                }
            }

            // Print end line.
            std::cout << std::endl;
        }

        // TEST: demonstration of low rank colour image approximation using SVD.
        void test_lowRankApproxC(std::string fileName)
        {
            // Open file as colour image.
            auto image = cv::imread(fileName);

            // Check whether the file was a valid image.
            if (image.data == nullptr)
                throw "Could not open or find image file.";

            // Get the smaller matrix dimension (maximal possible rank).
            auto n = std::min(image.rows, image.cols);

            // Vector of 3 RGB matrices.
            std::vector<cv::Mat> rgb;

            // Split image into RGB channels.
            cv::split(image, rgb);

            // Scaling factor.
            auto scale = 255.;

            // Transform each RGB image into a float matrix.
            rgb[0].convertTo(rgb[0], CV_32F, 1. / scale);
            rgb[1].convertTo(rgb[1], CV_32F, 1. / scale);
            rgb[2].convertTo(rgb[2], CV_32F, 1. / scale);

            // Perform SVD on RGB image matrices.
            cv::SVD svdR(rgb[2], cv::SVD::MODIFY_A);
            cv::SVD svdG(rgb[1], cv::SVD::MODIFY_A);
            cv::SVD svdB(rgb[0], cv::SVD::MODIFY_A);

            // Reset colour image matrix.
            image = cv::Mat(image.size(), CV_32FC3);

            // Perform sum of 1-rank approximation matrices.
            for (auto r = 1; r <= n; ++r)
            {
                // Reset RGB vector.
                rgb.clear();

                // Reconstruct RGB components.
                rgb.push_back(svdB.u(cv::Range::all(), cv::Range(r - 1, r)) * svdB.w(cv::Range(r - 1, r), cv::Range::all()) * svdB.vt(cv::Range(r - 1, r), cv::Range::all()));
                rgb.push_back(svdG.u(cv::Range::all(), cv::Range(r - 1, r)) * svdG.w(cv::Range(r - 1, r), cv::Range::all()) * svdG.vt(cv::Range(r - 1, r), cv::Range::all()));
                rgb.push_back(svdR.u(cv::Range::all(), cv::Range(r - 1, r)) * svdR.w(cv::Range(r - 1, r), cv::Range::all()) * svdR.vt(cv::Range(r - 1, r), cv::Range::all()));

                // i-th approximation matrix.
                cv::Mat sum(image.size(), image.type());

                // Merge RGB channels into 1 image matrix.
                cv::merge(rgb, sum);

                // Add i-th approximation.
                image += sum;

                // Show the i-rank approximation.
                cv::imshow("LRA", image);

                // Print approximation percentage.
                std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(1) << "\r" << (static_cast<float>(r) / n) * 100. << "%";

                // Print compression percentage.
                std::cout << " (" <<  static_cast<float>(r) * (image.rows + image.cols + 1.) / (image.rows * image.cols) * 100. << "%)";

                // Wait for key press and terminate if was 'q'.
                if (cv::waitKey() == 'q')
                    break;;
            }

            // Print end line.
            std::cout << std::endl;
        }

        // TEST: demonstration of grey scale image reconstruction using SVD.
        void test_svdG(std::string fileName)
        {
            // Open file as grey scale image.
            auto image = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);

            // Check whether the file was a valid image.
            if (image.data == nullptr)
                throw "Could not open or find image file.";

            // Get the smaller matrix dimension (maximal possible rank).
            auto n = std::min(image.rows, image.cols);

            // Transform the image into a float matrix.
            image.convertTo(image, CV_32F, .004f);

            // Perform SVD on image matrix.
            cv::SVD svd(image, cv::SVD::MODIFY_A);

            // Get singular values matrix Sigma from vector 'svd.w'.
            cv::Mat s(n, n, CV_32F);
            for (auto i = 0; i < n; ++i)
                s.at<cv::Vec<float, 1>>(i, i) = svd.w.at<cv::Vec<float, 1>>(i);

            // Recompose image as U * S * V'.
            cv::Mat result = svd.u * s * svd.vt;

            // Show reconstructed image.
            cv::imshow("SVD", result);
            while (cv::waitKey() != 'q');
        }

        // TEST: split image into RGB and SVD and reconstruct it.
        void test_svdC(std::string fileName)
        {
            // Open file as colour image.
            auto image = cv::imread(fileName);

            // Check whether the file was a valid image.
            if (image.data == nullptr)
                throw "Could not open or find image file.";

            // Get the smaller matrix dimension (maximal possible rank).
            auto n = std::min(image.rows, image.cols);

            // Vector of 3 RGB matrices.
            std::vector<cv::Mat> rgb;

            // Split image into RGB channels.
            cv::split(image, rgb);

            // Scaling factor.
            auto scale = 255.;

            // Transform each RGB image into a float matrix.
            rgb[0].convertTo(rgb[0], CV_32F, 1. / scale);
            rgb[1].convertTo(rgb[1], CV_32F, 1. / scale);
            rgb[2].convertTo(rgb[2], CV_32F, 1. / scale);

            // Perform SVD on RGB image matrices.
            cv::SVD svdR(rgb[2], cv::SVD::MODIFY_A);
            cv::SVD svdG(rgb[1], cv::SVD::MODIFY_A);
            cv::SVD svdB(rgb[0], cv::SVD::MODIFY_A);

            // Get singular values matrices.
            cv::Mat sR(n, n, CV_32F), sG(n, n, CV_32F), sB(n, n, CV_32F);
            for (auto i = 0; i < n; ++i)
            {
                sR.at<cv::Vec<float, 1>>(i, i) = svdR.w.at<cv::Vec<float, 1>>(i);
                sG.at<cv::Vec<float, 1>>(i, i) = svdG.w.at<cv::Vec<float, 1>>(i);
                sB.at<cv::Vec<float, 1>>(i, i) = svdB.w.at<cv::Vec<float, 1>>(i);
            }

            // Reset RGB vector.
            rgb.clear();

            // Reconstruct RGB components.
            rgb.push_back(svdB.u * sB * svdB.vt);
            rgb.push_back(svdG.u * sG * svdG.vt);
            rgb.push_back(svdR.u * sR * svdR.vt);

            // Convert float matrices to bytes matrices.
            rgb[0].convertTo(rgb[0], CV_8U, scale);
            rgb[1].convertTo(rgb[1], CV_8U, scale);
            rgb[2].convertTo(rgb[2], CV_8U, scale);

            // Merge RGB channels into 1 image matrix.
            cv::merge(rgb, image);

            // Show reconstructed image.
            cv::imshow("RGB", image);
            while (cv::waitKey() != 'q');
        }

        // TEST: split camera stream into RGB channels.
        void test_rgbCam()
        {
            // Try to open the default camera.
            cv::VideoCapture capture(CV_CAP_ANY);

            // Check whether could open the default camera.
            if (!capture.isOpened())
                throw "Could not open or find video file.";

            // Loop forever.
            while (true)
            {
                // Get a new frame from camera.
                cv::Mat frame;
                capture >> frame;

                // The frame must not be empty.
                if (!frame.empty())
                {
                    // Vector of 3 RGB matrices.
                    std::vector<cv::Mat> rgb;

                    // Split image into RGB channels.
                    cv::split(frame, rgb);

                    // Empty matrix of the same size as frame.
                    cv::Mat R, G, B, empty = cv::Mat::zeros(frame.size(), CV_8U);

                    // Vector containing the channels of the new coloured image.
                    std::vector<cv::Mat> channels;

                    // Construct red image.
                    channels.push_back(empty);
                    channels.push_back(empty);
                    channels.push_back(rgb[2]);
                    cv::merge(channels, R);

                    // Construct green image.
                    channels.clear();
                    channels.push_back(empty);
                    channels.push_back(rgb[1]);
                    channels.push_back(empty);
                    cv::merge(channels, G);

                    // Construct blue image.
                    channels.clear();
                    channels.push_back(rgb[0]);
                    channels.push_back(empty);
                    channels.push_back(empty);
                    cv::merge(channels, B);

                    // Show all images.
                    cv::imshow("Original", frame);
                    cv::imshow("R", R);
                    cv::imshow("G", G);
                    cv::imshow("B", B);

                    // Wait until key press.
                    if (cv::waitKey(25) == 'q')
                        break;
                }
            }
        }
    }
}