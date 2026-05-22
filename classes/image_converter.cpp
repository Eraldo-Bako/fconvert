// fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "image_converter.h"
#include "path_handler.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <set>

void image_convert_logic(fs::path in, std::string fmt, bool silent) {
    PathHandler::log("[-] Status: Initializing image conversion. [-]");
    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    PathHandler::log("[-] Status: Reading the image file. [-]");
    cv::Mat img = cv::imread(in.string(), cv::IMREAD_UNCHANGED);
    if (img.empty()) return;

    PathHandler::log("[-] Status: Converting to fromat:'" + fmt + "' [-]");
    std::vector<int> p;
    if (fmt == "jpg" || fmt == "jpeg") { 
        p.push_back(cv::IMWRITE_JPEG_QUALITY); 
        p.push_back(95); 
    } else if (fmt == "webp") {
        p.push_back(cv::IMWRITE_WEBP_QUALITY);
        p.push_back(85);
    }

    PathHandler::log("[-] Status: Writing the file. [-]");
    if (cv::imwrite(out.string(), img, p)) {
        std::cout << " [+] Saved: " << out.filename() << std::endl;
    }
}

void image() {
    std::string name, fmt;
    std::cout << "Image filename or path: ";
    if(!(std::getline(std::cin >> std::ws, name))) {
        PathHandler::log("[!] Error: No valid input provided! [!]");
        PathHandler::log("[-] Status: Clearing flags and exiting. [-]");
        std::cin.clear();
        return;
    }
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        PathHandler::log("[!] Error: Path could not be resolved. [!]");
        std::cout << " [!] File not found. [!]\n";
        return;
    }

    std::cout << "Target Format: "; 
    if(!(std::getline(std::cin >> std::ws, fmt))) {
        PathHandler::log("[!] Error: No valid input provided! [!]");
        PathHandler::log("[~] Status: Clearing flags and exiting. [~]");
        std::cin.clear();
        return;
    } // safe lowercase conversion down below
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        PathHandler::log("[~] Detected: " + fmt + "\nQuitting... [~]");
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
}