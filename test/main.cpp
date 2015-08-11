// Define debug mode.
#include "settings.h"

// Standard C++.
#include <iostream>
#ifdef CGI
#include <fstream>
#include <sstream>
#include <ctime>
#endif

// Project headers.
#include "NSTU.h"
#include "Utils/Utils.h"
#ifdef CGI
#include "CGI/MultipartRequest.h"
#endif

#ifndef CGI

int main(int argc, char* argv[])
{
    std::cout << std::endl << "NSTU v" << VERSION << std::endl;
    std::cout << "Copyright (C) 2013 Fabio M. Banfi (fbanfi90@gmail.com)" << std::endl << std::endl;

    // Make sure the right number of parameters has been passed.
    if (!(argc == 3 && strlen(argv[1]) >= 2 && (argv[1][1] == 'c' || argv[1][1] == 'u') || argc == 4 && strlen(argv[1]) >= 2 && (argv[1][1] == 'i' || argv[1][1] == 'v')))
    {
        std::cout << "Usage: NSTU [[-i | -v] input_file output_text_file | -c output_text_file]." << std::endl;
        return -1;
    }

    // Identify chosen option.
    switch (argv[1][1])
    {
    case 'i': // Process image file.
        try { NSTU::processImage(std::string(argv[2]), std::string(argv[3])); }
        catch (char* e) { std::cout << e << std::endl; }
        break;

    case 'v': // Process video file.
        try { NSTU::processVideo(std::string(argv[2]), std::string(argv[3])); }

        catch (char* e) { std::cout << e << std::endl; }
        break;

    case 'c': // Process cam stream.
        try { NSTU::processCamera(std::string(argv[2])); }
        catch (char* e) { std::cout << e << std::endl; }
        break;

    case 'u': // TEST: test from NSTU::Util.
        try { NSTU::Utils::test_lowRankApproxC(std::string(argv[2])); }
        catch (char* e) { std::cout << e << std::endl; }
        break;

    default: // Not a valid option.
        std::cout << "Option not recognized." << std::endl;
    }

    return 0;
}

#elif defined(POST)

int main()
{
    // The error code.
    std::string error;

    // Process the multipart request.
    MultipartRequest req("../../upload/");

    // Check for errors.
    if (req.errorCode() == MultipartRequest::MPR_NO_ERROR)
    {
        // Get a pointer to the data file element.
        MultipartRequest::FormElement* pElement = req["file"];

        // Check whether the file was found.
        if (pElement)
        {
            // Check whether the file is valid.
            if (pElement->fileName())
            {
                // Create a string representing the current date and time.
                time_t rawtime;
                tm* timeinfo;
                char currentTime[16];
                time(&rawtime);
                timeinfo = localtime(&rawtime);
                strftime(currentTime, 16, "%y%m%d%H%M%S", timeinfo);

                // Set output file name.
                std::string outFileName(pElement->fileName());

                // Remove directory if present.
                const auto lastSlashIndex = outFileName.find_last_of("\\/");
                if (lastSlashIndex != std::string::npos)
                    outFileName.erase(0, lastSlashIndex + 1);

                // Remove extension.
                const auto periodIndex = outFileName.rfind('.');
                if (periodIndex != std::string::npos)
                    outFileName.erase(periodIndex);

                // Create the path string for the output file.
                std::ostringstream oss;
                oss << "../../download/" << outFileName << "_" << currentTime << ".xml";
                std::string outFilePath = oss.str();

                // Check wheter file is an image.
                if (std::string(pElement->type()) == std::string("image/jpeg"))
                {
                    try
                    {
                        // Process the image file.
                        NSTU::processImage(std::string(pElement->localFileName()), std::string(outFilePath));

                        // If no exception was raised the operation was successful, hence download the file.
                        std::cout << "Content-disposition: attachment; filename=" << outFileName << ".xml\r\n\r\n";

                        // Write XML to responce.
                        std::string line;
                        std::ifstream ifs(outFilePath);
                        while (std::getline(ifs, line))
                            std::cout << line << "\n";

                        // Terminate successfully.
                        return 0;
                    }

                    // Set the error code to the raised exception.
                    catch (char* e) { error = std::string(e); }
                }
                
                // Check whether file is a video.
                else if (std::string(pElement->type()) == std::string("video/avi"))
                {
                    try
                    {
                        // Process the image file.
                        NSTU::processVideo(std::string(pElement->localFileName()), std::string(outFilePath));

                        // If no exception was raised the operation was successful, hence download the file.
                        std::cout << "Content-disposition: attachment; filename=" << outFileName << ".xml\r\n\r\n";

                        // Write XML to response.
                        std::string line;
                        std::ifstream ifs(outFilePath);
                        while (std::getline(ifs, line))
                            std::cout << line << "\n";

                        // Terminate successfully.
                        return 0;
                    }

                    // Set the error code to the raised exception.
                    catch (char* e) { error = std::string(e); }
                }

                // Not a valid option.
                else
                    error = "Format not supported.";
            }

            // Inform that the file is not valid.
            else
                error = "File not valid.";
        }
        
        // Inform that the file was not found.
        else
            error = "File not found.";
    }

    // Handle the error if processing the multipart request failed.
    else
    {
        switch(req.errorCode())
        {
        case MultipartRequest::IO_ERROR: // I/O error occured.
            error = "IO error.";
            break;

        case MultipartRequest::PARSE_ERROR: // Multipar request parse error.
            error = "Parse error.";
            break;

        case MultipartRequest::MAX_UPLOAD: // Maximum file upload size.
            error = "Max upload size reached.";
            break;

        case MultipartRequest::CGI_ERROR: // CGI error.
            error = "CGI error.";
            break;
        }
    }
    
    // Send error as HTML page to the browser.
    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << "<!DOCTYPE html>\n";
    std::cout << "<html lang=\"en\">\n";
    std::cout << "<head>\n";
    std::cout << "<title>NSTU</title>\n";
    std::cout << "<link href=\"../../images/favicon.ico?v=1\" rel=\"shortcut icon\" />\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";
    std::cout << "<p>\n";
    std::cout << error << "\n";
    std::cout << "</p>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";

    // Terminate with error.
    return -1;
}

#elif defined(GET)

int main()
{
    // Get the environment variables.
#ifndef NSTU_DEBUG
#if defined(WIN32) || defined(WIN64)
    size_t size = 64;
    char data[64];
    getenv_s(&size, data, "QUERY_STRING");
#else
    char* data = getenv("QUERY_STRING");
#endif
#else
    char* data = "option=i&filename=signs.jpg";
    //char* data = "invalid_input";
#endif

    // Send HTML to the browser.
    std::cout << "Content-type:text/html\r\n\r\n";
    std::cout << "<!DOCTYPE html>\n";
    std::cout << "<html lang=\"en\">\n";
    std::cout << "<head>\n";
    std::cout << "<title>NSTU</title>\n";
    std::cout << "<link href=\"../../images/favicon.ico?v=1\" rel=\"shortcut icon\" />\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";
    std::cout << "<p>\n";

    // Could not read query string.
    if (data == nullptr)
        std::cout << "Data not transmitted." << std::endl;

    // Parse the query string.
    else
    {
        // Create a string stream from the environment variables char array.
        std::istringstream iss(data);

        // The strings to be parsed.
        std::string name;
        std::string option;
        std::string fileName;

        // Parse the string stream.
        std::getline(iss, name, '=');
        std::getline(iss, option, '&');
        std::getline(iss, name, '=');
        std::getline(iss, fileName);

        // Show parsed strings.
#ifdef NSTU_DEBUG
        std::cout << option << std::endl;
        std::cout << fileName << std::endl;
#endif

        // Check wheter parsing was successful.
        if (option.length() == 1 && fileName != "")
        {
            // Create a string representing the current date and time.
            time_t rawtime;
            tm* timeinfo;
            char currentTime[16];
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(currentTime, 16, "%y%m%d%H%M%S", timeinfo);

            // Set output file name.
            std::string outFileName(fileName);

            // Remove directory if present.
            const auto lastSlashIndex = outFileName.find_last_of("\\/");
            if (lastSlashIndex != std::string::npos)
                outFileName.erase(0, lastSlashIndex + 1);

            // Remove extension.
            const auto periodIndex = outFileName.rfind('.');
            if (periodIndex != std::string::npos)
                outFileName.erase(periodIndex);

            // Create the path string for the output file.
            std::ostringstream oss;
            oss << "../../download/" << outFileName << "_" << currentTime << ".xml";
            std::string outPath = oss.str();

            // Display the output file path.
#ifdef NSTU_DEBUG
            std::cout << outPath << std::endl;
#endif

            // Identify chosen option.
            switch (option[0])
            {
            case 'i': // Process image file.
                try
                {
                    NSTU::processImage(std::string(fileName), std::string(outPath));
                    std::cout << "Operation completed successfully." << std::endl;
                }
                catch (char* e) { std::cout << e << std::endl; }
                break;

            case 'v': // Process video file.
                try
                {
                    NSTU::processVideo(fileName, std::string(outPath));
                    std::cout << "Operation completed successfully." << std::endl;
                }
                catch (char* e) { std::cout << e << std::endl; }
                break;

            default: // Not a valid option.
                std::cout << "Option not recognized." << std::endl;
            }
        }

        // The input was malformed.
        else
            std::cout << "Invalid data." << std::endl;
    }

    // Wait for keypress.
#ifdef NSTU_DEBUG
    getchar();
#endif

    // Send HTML to the browser.
    std::cout << "</p>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";

    return 0;
}

#endif

// C++11 features not supported by VS12:
//auto vec = std::vector<int>({2, 4, 6, 8, 10});
//std::initializer_list
//for (int i : {-1, -2, -3}) std::cout << i << std::endl;
