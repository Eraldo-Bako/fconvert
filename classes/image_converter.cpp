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
#include <fmt/core.h>

// sudo pacman -S libraw imagemagick ghostscript 
// AUR: vtracer
// codecs: libheif libde265 x265
cv::Mat read_camera_raw(const std::string& raw_path) {

    if (!Program::Check::libraw()) { // if LibRaw is not present/deleted at runtime
        Program::print(_("[!] Error: LibRAW not found. [!]\n"), Program::PrintType::Error);
        Program::end(_("[!] This part of the program cannot function without LibRAW. [!]"));
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

void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent, Image::SVG vector_precision) {

    if (!Program::Check::opencv()) {
        Program::print(_("[!] Error: OpenCV not found. [!]\n"), Program::PrintType::Error);
        return;
    }

    Program::log(_("[-] Status: Initializing image conversion. [-]"));
    std::filesystem::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    SecureConversionSession session(in, fmt);

    Program::log(_("[-] Status: Reading the image file. [-]"));
    cv::Mat img;

    std::string in_ext = std::filesystem::path(session.safe_input()).extension().string();
    std::transform(in_ext.begin(), in_ext.end(), in_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (in_ext == ".cr2" || in_ext == ".nef" || in_ext == ".arw" || in_ext == ".dng" || in_ext == ".crw") {
#ifdef HAS_LIBRAW
        if (!Program::Check::libraw()) {
            Program::print(_("[!] Error: LibRAW not found. [!]\n"), Program::PrintType::Error);
            return;
        }
        Program::log(_("[-] Status: Developing Camera RAW file via LibRaw. [-]"));
        img = read_camera_raw(session.safe_input());
#else // built w/ -DWITH_LIBRAW=OFF
        Program::print(_("[!] Error: This binary was built without LibRAW support. [!]\n"), Program::PrintType::Error);
        Program::end(_("[!] RAW image loading is disabled. [!]"));
        return; //never reached bc of end() but good to have
#endif
    } else if (in_ext == ".psd" || in_ext == ".svg" || in_ext == ".pdf" || 
               in_ext == ".gif" || in_ext == ".ico" || in_ext == ".xcf" || 
               in_ext == ".eps" || in_ext == ".ai" || in_ext == ".heic" || in_ext == ".heif") {
        
        Program::log(_("[-] Status: OpenCV cannot read this format. Invoking foreign fallback reader... [-]"));
        std::string temp_in_png = (std::filesystem::path(session.safe_input()).parent_path()/ "temp_in_holder.png").string();
        
        std::string cmd;
        // magick
        if (in_ext != ".gif") cmd = Program::Build::command(Program::Build::cmdType::Magick, session.safe_input(), temp_in_png, "layered"); // general
        else cmd = Program::Build::command(Program::Build::cmdType::Magick, session.safe_input(), temp_in_png, "fast"); // for gifs only frame 0

        int result = std::system(cmd.c_str());
        
        if (result == 0 && std::filesystem::exists(temp_in_png)) {
            img = cv::imread(temp_in_png, cv::IMREAD_UNCHANGED);
            std::filesystem::remove(temp_in_png);
        } else {
            Program::print(fmt::format(_("[!] Error: ImageMagick failed to read '{0}'. [!]\n"), in_ext));
        }
    } else {
        img = cv::imread(session.safe_input(), cv::IMREAD_UNCHANGED);
    }

    if (img.empty()) {
        Program::log(_("[!] Error: Image data is empty or corrupt. [!]"));
        return;
    }

    Program::log(fmt::format(_("[-] Status: Converting to format:'{0}' [-]"), fmt));
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
    if (external_write) { // fallback using imagemagic + opencv or vtracer

        Program::log(_("[-] Status: Prepearing for external converter. [-]"));
        Program::log(_("[-] Status: Writing temporary file. [-]"));
        std::string temp_png = (std::filesystem::path(session.safe_output()).parent_path() / "temp_holder.png").string();
        cv::imwrite(temp_png, img); // temp png using opencv
        std::string externalType;
        std::string cmd;

        if (vector_precision != Image::SVG::INACTIVE) { // vtracer

            if (!Program::Check::vtracer()) {
                Program::print(_("[!] Error: VTracer not found. [!]\n"), Program::PrintType::Error);
                return;
            }

            std::string tempParams;
            switch (vector_precision) {
                case Image::SVG::VECTOR:
                    tempParams = "--colormode bw";
                    break;
                case Image::SVG::CVECTOR:
                    tempParams = "--colormode color --color_precision 8 --gradient_step 80 --filter_speckle 16";
                    break;
                case Image::SVG::BIMGV:
                    tempParams = "--colormode color --color_precision 8 --gradient_step 20 --filter_speckle 8";
                    break;
                case Image::SVG::PIMGV:
                    tempParams = "--colormode color --color_precision 8 --gradient_step 1 --filter_speckle 1";
                    break;
                case Image::SVG::CUSTOM: {
                    Program::log(_("[-] Status: Initiating custom profile setting. [-]"));

                    bool color;
                    std::string userChoice = Program::Get::input(_("Do you want color in your vector tracing? [Y/n] "), Program::Case::Lower, Program::InputType::noWS);
                    if (userChoice.empty()) color = true;
                    else color = (userChoice[0] != 'n');
                    
                    if (color) {
                        tempParams = "--colormode color";

                        int color_precision = 8;
                        while(true) {
                            std::string input = Program::Get::input(_("Specify the color precision in vector tracing [1-8]: "), Program::Case::Lower, Program::InputType::noWS);
                            if (input.empty()) break;
                            
                            if (input == "q" || input == "quit" || input == "exit" || input == "cancel") {
                                Program::end(_("[!!] Operation quit by user. Exiting... [!!]"));
                                break; // doesnt resolve
                            }

                            try {
                                color_precision = std::stoi(input);
                                if (color_precision >= 1 && color_precision <= 8) break;
                            } catch (...) {}
                            Program::print(_("[!] Error: Color Precision needs to be a value between 1-8. [!]"), Program::PrintType::Error);
                        }
                        tempParams += " --color_precision " + std::to_string(color_precision);

                        int gradient_step = 20;
                        while(true) {
                            std::string input = Program::Get::input(_("Specify the gradient step in vector tracing [0-255]: "), Program::Case::Lower, Program::InputType::noWS);
                            if (input.empty()) break;
                            
                            if (input == "q" || input == "quit" || input == "exit" || input == "cancel") {
                                Program::end(_("[!!] Operation quitted by user. Exiting... [!!]"));
                                break; // doesnt resolve
                            }

                            try {
                                gradient_step = std::stoi(input);
                                if (gradient_step >= 0 && gradient_step <= 255) break;
                            } catch (...) {}
                            Program::print(_("[!] Error: Gradient Step needs to be a value between 0-255. [!]"), Program::PrintType::Error);
                        }
                        tempParams += " --gradient_step " + std::to_string(gradient_step);

                        int filter_speckle = 8;
                        while(true) {
                            std::string input = Program::Get::input(_("Specify the amount of filter speckle in vector tracing [0-16]: "), Program::Case::Lower, Program::InputType::noWS);
                            if (input.empty()) break;
                            
                            if (input == "q" || input == "quit" || input == "exit" || input == "cancel") {
                                Program::end(_("[!!] Operation quit by user. Exiting... [!!]"));
                                break; // doesnt resolve
                            }

                            try {
                                filter_speckle = std::stoi(input);
                                if (filter_speckle >= 0 && filter_speckle <= 16) break;
                            } catch (...) {}
                            Program::print(_("[!] Error: Filter Speckle needs to be a value between 0-16. [!]"), Program::PrintType::Error);
                        }
                        tempParams += " --filter_speckle " + std::to_string(filter_speckle);
                    } else tempParams = "--colormode bw";
                    break;
                }
                default: break;
            }
            externalType = "VTracer";
            cmd = Program::Build::command(Program::Build::cmdType::VTracer, temp_png, session.safe_output(), tempParams);
        } else { //magick
            if (!Program::Check::imghost()) {
                Program::print(_("[!] Error: ImageMagick or Ghostscript not found. [!]\n"), Program::PrintType::Error);
                return;
            }

            externalType = "ImageMagick";
            
            Program::log(_("[-] Status: Writing the file. [-]"));
            if (in_ext != ".gif") cmd = Program::Build::command(Program::Build::cmdType::Magick, temp_png, session.safe_output(), "layered");
            else cmd = Program::Build::command(Program::Build::cmdType::Magick, temp_png, session.safe_output(), "fast"); // gifs
        }
        
        int result = std::system(cmd.c_str()); // using imagemagick to wrap it into svg
        Program::log(_("[-] Status: Cleaning temporary files. [-]"));
        std::filesystem::remove(temp_png);

        if (result != 0) 
            Program::print(
                fmt::format(
                    _("[!] Error: {foreign} failed to convert to '{format}' [!]\n"),
                    fmt::arg("foreign", externalType),
                    fmt::arg("format", fmt)
                ), 
                Program::PrintType::Error
            );
        else {
            Program::print(fmt::format(_("[+] Saved Wrapped Image: {0} [+]\n"), session.safe_output()));
            conversion_success = true;
        }
    } else {
        Program::log(_("[-] Status: Writing the file. [-]"));
        if (cv::imwrite(session.safe_output(), img, p)) {
            Program::print(fmt::format(_("[+] Saved: {0} [+]\n"), session.safe_output()));
            conversion_success = true;
        }
    }

    if (conversion_success) {

        if (session.commit(out)) Program::print(fmt::format(_("[+] Saved: {0} [+]\n"), out.filename().string()));
        else Program::print(_("[!!] Error: Failed to safely export output file from the internal sandbox. [!!]\n"), Program::PrintType::Error);

    } else Program::print(_("[!!] Error: Image writing or processing failed. [!!]\n"), Program::PrintType::Error);

}

void image() {

    std::string name = Program::Get::input(_("Image filename or path: "));
    std::filesystem::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        Program::log(_("[!] Error: Path could not be resolved. [!]"));
        Program::print(_("[!] File not found. [!]\n"), Program::PrintType::Error);
        std::cerr << std::flush;
        return;
    }

    std::string fmt = Program::Get::input(_("Format: "), Program::Case::Lower);
    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        Program::log(fmt::format(_("[~] Detected: {0} [~]\n[~] Quitting... [~]"), fmt));
        std::cout << _("[!] Successfully stopped the conversion! [!]");
        return;
    }

    {    
        bool targetIsRAWImage = (fmt == "cr2" || fmt == "nef" || fmt == "arw" || 
                                fmt == "dng" || fmt == "crw");

        if (targetIsRAWImage) {
            Program::log(_("[!] Warning: Converting to a RAW image format is not supported, nor recommended! [!]"));
            Program::print(fmt::format(_("[!] Error: Detected target is a RAW image format: '{}' [!]"), fmt), Program::PrintType::Error);
            return;
        }
    }

    static const std::set<std::string> valid_image = {
        "jpeg", "jpg", "png", "webp", "gif", "tiff", "tif", "bmp",
        "svg", "psd", "pdf", "ico", "xcf", "eps", "ai", "heic", "heif"
    };
    if (valid_image.find(fmt) == valid_image.end()) {
        std::cout << fmt::format(_("\n[!] Format '{0}' is not supported or doesn't exist. [!]\n"), fmt)
                  << _("Supported image formats include:\n")
                  << "JPEG/JPG, PNG, WebP, GIF, TIFF/TIF, BMP, HEIC, SVG, PSD, PDF, ICO, XCF, EPS, AI\n"
                  << _("RAW Image formats should never be your target format.\n")
                  << _("\n[-] If you believe this is a bug, make sure to report it. [-]\n");
        return;
    }
    
    Image::SVG SVG_Precision = Image::SVG::INACTIVE; // when non-vector conversion
    {
        bool targetIsVector = (fmt == "svg" || fmt == "ai");
        if (targetIsVector) {
            Program::log(_("[-] Detected: Converting to a vector-based format! [-]"));
            std::string prompt = 
            fmt::format(
                _("[-] Converting to {0}. [-]\n"), 
                fmt
            ) +
            _("[-] Select Profile: \n") +
            _("    1  Vector Tracing\n") +
            _("    2  Colorful Vector Tracing\n") +
            _("   [3] Basic Image Precision\n") +
            _("    4  Perfect Image Precision\n") +
            _("    5  Custom\n") +
            _("Choice [3]: ");

            while (true) {
                std::string input = Program::Get::input(prompt, Program::Case::Lower, Program::InputType::noWS);

                if (input.empty()) {
                    SVG_Precision = Image::SVG::BIMGV;
                    break;
                }

                if (input == "q" || input == "quit" || input == "exit" || input == "cancel") {
                    SVG_Precision = Image::SVG::QUIT;
                    break;
                }

                bool valid_choice = true;
                try {
                    int choice = std::stoi(input);
                    switch (choice) {
                        case 1:
                            SVG_Precision = Image::SVG::VECTOR;
                            break;
                        case 2:
                            SVG_Precision = Image::SVG::CVECTOR;
                            break;
                        case 3:
                            SVG_Precision = Image::SVG::BIMGV;
                            break;
                        case 4:
                            SVG_Precision = Image::SVG::PIMGV;
                            break;
                        case 5:
                            SVG_Precision = Image::SVG::CUSTOM;
                            break;
                        default:
                            valid_choice = false;
                            break;
                    }
                } catch (...) {
                    valid_choice = false;
                }

                if (valid_choice) break;

                Program::log(_("[!] Invalid selection. Please enter a number between 1 and 5. [!]"));
            }
        }
    }

    if (SVG_Precision != Image::SVG::QUIT) image_convert_logic(in, fmt, false, SVG_Precision);
}