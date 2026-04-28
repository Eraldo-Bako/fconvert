#include "image_converter.h"
#include "path_handler.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <set>

void image_convert_logic(fs::path in, std::string fmt, bool silent) {
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), ::tolower);
    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    cv::Mat img = cv::imread(in.string(), cv::IMREAD_UNCHANGED);
    if (img.empty()) return;

    std::vector<int> p;
    if (fmt == "jpg" || fmt == "jpeg") { 
        p.push_back(cv::IMWRITE_JPEG_QUALITY); 
        p.push_back(95); 
    } else if (fmt == "webp") {
        p.push_back(cv::IMWRITE_WEBP_QUALITY);
        p.push_back(85);
    }
    
    if (cv::imwrite(out.string(), img, p)) {
        std::cout << " [+] Saved: " << out.filename() << std::endl;
    }
}

void image() {
    std::string name, fmt;
    std::cout << "Image filename or path: "; std::getline(std::cin >> std::ws, name);
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) return;

    std::cout << "Format: "; if (!(std::cin >> fmt)) return;
    for (auto &c : fmt) c = std::tolower(c);

    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    static const std::set<std::string> valid_image = {"jpeg", "jpg", "png", "webp", "gif", "tiff", "bmp"};
    if (valid_image.find(fmt) == valid_image.end()) {
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported image formats include:\n"
                  << "JPEG/JPG, PNG, WebP, GIF, TIFF, BMP\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }

    image_convert_logic(in, fmt, false);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}