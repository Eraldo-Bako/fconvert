// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "image_converter.hpp"
#include "path_handler.hpp"
#include "program_handler.hpp"
#include "secure_conversion_session.hpp"

#include <opencv2/imgcodecs.hpp> // cv::imread, cv::imwrite, and IMWRITE flags
#include <opencv2/imgproc.hpp>   // cv::cvtColor and cv::COLOR_RGB2BGR

#ifdef HAS_LIBRAW
#ifdef _WIN32
    #include <libraw.h>
#else
    #include <libraw/libraw.h>
#endif
#endif

#include <iostream>
#include <algorithm>
#include <cctype>
#include <set>

// sudo pacman -S libraw imagemagick ghostscript 
// codecs: libheif libde265 x265
cv::Mat read_camera_raw(const std::string& raw_path) {

    if (!Program::Check::libraw()) { // if LibRaw is not present/deleted at runtime
        Program::print("[!] Error: LibRAW not found. [!]\n", Program::PrintType::Error);
        Program::end("[!] This part of the program cannot function without LibRAW. [!]");
        return cv::Mat(); //shouldnt run, Program::end must always succeed!!! if it does run...something is very wrong...
    }

    LibRaw processor;
    if (processor.open_file(raw_path.c_str()) != LIBRAW_SUCCESS) return cv::Mat();
    if (processor.unpack() != LIBRAW_SUCCESS) return cv::Mat();

    processor.imgdata.params.output_bps = 8; // 8-bit channels for standard cv::Mat
    if (processor.dcraw_process() != LIBRAW_SUCCESS) return cv::Mat();

    int error_code = 0;
    libraw_processed_image_t* image = processor.dcraw_make_mem_image(&error_code);
    if (!image || error_code != 0) return cv::Mat();

    // wraping the memory buffer into an OpenCV Mat (LibRaw==>RGB==>BGR==>OpenCV)
    cv::Mat rgb_mat(image->height, image->width, CV_8UC3, image->data);
    cv::Mat bgr_mat;
    cv::cvtColor(rgb_mat, bgr_mat, cv::COLOR_RGB2BGR);

    processor.dcraw_clear_mem(image); // cleaning up LibRaw memory
    processor.recycle();

    return bgr_mat; // full-res image matrix
}

void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent) {

    if (!Program::Check::opencv()) {
        Program::print("[!] Error: OpenCV not found. [!]\n", Program::PrintType::Error);
        return;
    }

    Program::log("[-] Status: Initializing image conversion. [-]");
    std::filesystem::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    SecureConversionSession session(in, fmt);

    Program::log("[-] Status: Reading the image file. [-]");
    cv::Mat img;

    std::string in_ext = std::filesystem::path(session.safe_input()).extension().string();
    std::transform(in_ext.begin(), in_ext.end(), in_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (in_ext == ".cr2" || in_ext == ".nef" || in_ext == ".arw" || in_ext == ".dng" || in_ext == ".crw") {
#ifdef HAS_LIBRAW
        Program::log("[-] Status: Developing Camera RAW file via LibRaw. [-]");
        img = read_camera_raw(session.safe_input());
#else // built w/ -DWITH_LIBRAW=OFF
        Program::print("[!] Error: This binary was built without LibRAW support. [!]\n", Program::PrintType::Error);
        Program::end("[!] RAW image loading is disabled. [!]");
        return; //never reached bc of end() but good to have
#endif
    } else if (in_ext == ".psd" || in_ext == ".svg" || in_ext == ".pdf" || 
               in_ext == ".gif" || in_ext == ".ico" || in_ext == ".xcf" || 
               in_ext == ".eps" || in_ext == ".ai" || in_ext == ".heic" || in_ext == ".heif") {
        
        Program::log("[-] Status: OpenCV cannot read this format. Invoking ImageMagick fallback reader... [-]");
        std::string temp_in_png = (std::filesystem::path(session.safe_input()).parent_path()/ "temp_in_holder.png").string();
        
        std::string cmd = "magick \"" + session.safe_input() + "[0]\" \"" + temp_in_png + "\"";
        int result = std::system(cmd.c_str());
        
        if (result == 0 && std::filesystem::exists(temp_in_png)) {
            img = cv::imread(temp_in_png, cv::IMREAD_UNCHANGED);
            std::filesystem::remove(temp_in_png);
        } else {
            Program::print(" [!] Error: ImageMagick failed to read '" + in_ext + "'. [!]\n");
        }
    } else {
        img = cv::imread(session.safe_input(), cv::IMREAD_UNCHANGED);
    }

    if (img.empty()) {
        Program::log("[!] Error: Image data is empty or corrupt. [!]");
        return;
    }

    Program::log("[-] Status: Converting to format:'" + fmt + "' [-]");
    std::vector<int> p; // bpm* uses empty vector
    bool external_write = false;
    if (fmt == "jpg" || fmt == "jpeg") { 
        p.push_back(cv::IMWRITE_JPEG_QUALITY); 
        p.push_back(95); 
    } else if (fmt == "webp") {
        p.push_back(cv::IMWRITE_WEBP_QUALITY);
        p.push_back(85);
    } else if (fmt == "png") {
        p.push_back(cv::IMWRITE_PNG_COMPRESSION);
        p.push_back(3); // 0=none; 9=max-compression
    } else if (fmt == "tiff" || fmt == "tif") {
        p.push_back(cv::IMWRITE_TIFF_COMPRESSION);
        p.push_back(5); // 1=none, 5=lzw(lossless), 7=jpeg, 8=deflate(adobe)
    } else if (fmt == "heic" || fmt == "heif") {
        p.push_back(12000); // dynamic enum value for cv::IMWRITE_HEIF_QUALITY
        p.push_back(85);    // 85 balances compression n`visual detail
        
        #ifndef CV_IMWRITE_HEIF_QUALITY
        external_write = true; // fallback if OpenCV is missing codec
        #endif
    } else if (fmt == "svg" || fmt == "psd" || fmt == "pdf" 
            || fmt == "gif" || fmt == "ico" || fmt == "xcf" 
            || fmt == "eps" || fmt == "ai") external_write = true;

    bool conversion_success = false;
    if (external_write) { // fallback using imagemagic + opencv
        if (!Program::Check::imghost()) {
            Program::print("[!] Error: ImageMagick or Ghostscript not found. [!]\n", Program::PrintType::Error);
            return;
        }
        Program::log("[-] Status: Writing temporary file. [-]");
        std::string temp_png = (std::filesystem::path(session.safe_output()).parent_path() / "temp_holder.png").string();
        cv::imwrite(temp_png, img); // temp png using opencv
        
        Program::log("[-] Status: Writing the file. [-]");
        std::string cmd = "magick " + temp_png + " " + session.safe_output();
        int result = std::system(cmd.c_str()); // using imagemagick to wrap it into svg
        Program::log("[-] Status: Cleaning temporary files. [-]");
        std::filesystem::remove(temp_png);

        if (result != 0) Program::print(" [!] Error: ImageMagick failed to convert to '" 
                    + fmt + "' Make sure 'imagemagick' is installed. [!]\n", Program::PrintType::Error);
        else {
            Program::print("[+] Saved Wrapped Vector: " + session.safe_output() + " [+]\n");
            conversion_success = true;
        }
    } else {
        Program::log("[-] Status: Writing the file. [-]");
        if (cv::imwrite(session.safe_output(), img, p)) {
            Program::print("[+] Saved: " + session.safe_output() + " [+]\n");
            conversion_success = true;
        }
    }

    if (conversion_success) {

        if (session.commit(out)) Program::print("[+] Saved: " + out.filename().string() + " [+]\n");
        else Program::print("[!] Error: Failed to safely export output file from sandbox. [!]\n", Program::PrintType::Error);

    } else Program::print("[!] Error: Image writing or processing failed. [!]\n", Program::PrintType::Error);

}

void image() {

    std::string name = Program::Get::input("Image filename or path: ");
    std::filesystem::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        Program::log("[!] Error: Path could not be resolved. [!]");
        Program::print("[!] File not found. [!]\n", Program::PrintType::Error);
        std::cerr << std::flush;
        return;
    }

    std::string fmt = Program::Get::input("Format: ", Program::Case::Lower);
    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        Program::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    {    
        bool targetIsRAWImage = (fmt == "cr2" || fmt == "nef" || fmt == "arw" || 
                                fmt == "dng" || fmt == "crw");

        if (targetIsRAWImage) {
            Program::log("[!] Warning: Converting to a RAW image format is not supported, nor recommended! [!]");
            Program::print("[!] Error: Detected target is a RAW image format: '" + fmt + "' [!]", Program::PrintType::Error);
            return;
        }
    }

    static const std::set<std::string> valid_image = {
        "jpeg", "jpg", "png", "webp", "gif", "tiff", "tif", "bmp",
        "svg", "psd", "pdf", "ico", "xcf", "eps", "ai", "heic", "heif"
    };
    if (valid_image.find(fmt) == valid_image.end()) {
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported image formats include:\n"
                  << "JPEG/JPG, PNG, WebP, GIF, TIFF/TIF, BMP, HEIC, SVG, PSD, PDF, ICO, XCF, EPS, AI\n"
                  << "RAW Image formats should never be your target format.\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }

    image_convert_logic(in, fmt, false);
}