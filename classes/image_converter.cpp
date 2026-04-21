#include "image_converter.h"
#include "path_handler.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cctype>

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
    std::cout << "Image filename: "; std::getline(std::cin >> std::ws, name);
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) return;
    std::cout << "Format: "; std::cin >> fmt;
    image_convert_logic(in, fmt, false);
}