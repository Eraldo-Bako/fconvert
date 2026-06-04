// fconvert v2.2.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "image_converter.hpp"
#include "path_handler.hpp"

#include <opencv2/imgcodecs.hpp> // cv::imread, cv::imwrite, and IMWRITE flags
#include <opencv2/imgproc.hpp>   // cv::cvtColor and cv::COLOR_RGB2BGR

#include <iostream>
#include <algorithm>
#include <cctype>
#include <set>

// sudo pacman -S libraw imagemagick ghostscript 
// codecs: libheif libde265 x265
cv::Mat read_camera_raw(const std::string& raw_path) {
    LibRaw processor;
    if (processor.open_file(raw_path.c_str()) != LIBRAW_SUCCESS) return cv::Mat();
    if (processor.unpack() != LIBRAW_SUCCESS) return cv::Mat();

    processor.imgdata.params.output_bps = 8; // 8-bit channels for standard cv::Mat
    if (processor.dcraw_process() != LIBRAW_SUCCESS) return cv::Mat();

    int error_code = 0;
    libraw_processed_image_t* image = processor.dcraw_make_mem_image(&error_code);
    if (!image || error_code != 0) return cv::Mat();

    // wraps the memory buffer into an OpenCV Mat (LibRaw==>RGB==>BGR==>OpenCV)
    cv::Mat rgb_mat(image->height, image->width, CV_8UC3, image->data);
    cv::Mat bgr_mat;
    cv::cvtColor(rgb_mat, bgr_mat, cv::COLOR_RGB2BGR);

    processor.dcraw_clear_mem(image); // cleaning up LibRaw memory
    processor.recycle();

    return bgr_mat; // full-res image matrix
}

void image_convert_logic(fs::path in, std::string fmt, bool silent) {
    PathHandler::log("[-] Status: Initializing image conversion. [-]");
    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    PathHandler::log("[-] Status: Reading the image file. [-]");
    cv::Mat img;

    std::string in_ext = in.extension().string();
    std::transform(in_ext.begin(), in_ext.end(), in_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (in_ext == ".cr2" || in_ext == ".nef" || in_ext == ".arw" || in_ext == ".dng" || in_ext == ".crw") {
        PathHandler::log("[-] Status: Developing Camera RAW file via LibRaw. [-]");
        img = read_camera_raw(in.string());
    } else if (in_ext == ".psd" || in_ext == ".svg" || in_ext == ".pdf" || 
               in_ext == ".gif" || in_ext == ".ico" || in_ext == ".xcf" || 
               in_ext == ".eps" || in_ext == ".ai" || in_ext == ".heic" || in_ext == ".heif") {
        
        PathHandler::log("[-] Status: OpenCV cannot read this format. Invoking ImageMagick fallback reader... [-]");
        std::string temp_in_png = in.parent_path().string() + "/.fconvert_temp_in_holder.png";
        
        std::string cmd = "magick \"" + in.string() + "[0]\" \"" + temp_in_png + "\"";
        int result = std::system(cmd.c_str());
        
        if (result == 0 && std::filesystem::exists(temp_in_png)) {
            img = cv::imread(temp_in_png, cv::IMREAD_UNCHANGED);
            std::filesystem::remove(temp_in_png);
        } else {
            std::cerr << " [!] Error: ImageMagick failed to read '" << in_ext << "'. [!]" << std::endl;
        }
    } else {
        img = cv::imread(in.string(), cv::IMREAD_UNCHANGED);
    }

    if (img.empty()) {
        PathHandler::log("[!] Error: Image data is empty or corrupt. [!]");
        return;
    }

    PathHandler::log("[-] Status: Converting to fromat:'" + fmt + "' [-]");
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

    if (external_write) { // fallback using imagemagic + opencv
        PathHandler::log("[-] Status: Writing temporary file. [-]");
        std::string temp_png = out.parent_path().string() + "/.temp_holder.png";
        cv::imwrite(temp_png, img); // temp png using opencv
        
        PathHandler::log("[-] Status: Writing the file. [-]");
        std::string cmd = "magick " + temp_png + " " + out.string();
        int result = std::system(cmd.c_str()); // using imagemagick to wrap it into svg
        PathHandler::log("[-] Status: Cleaning temporary files. [-]");
        std::filesystem::remove(temp_png);

        if (result != 0) std::cerr << " [!] Error: ImageMagick failed to convert to '" 
                    << fmt << "' Make sure 'imagemagick' is installed. [!]" << std::endl;
        if (result == 0) std::cout << "[+] Saved Wrapped Vector: " 
                                   << out.filename() << " [+]" << std::endl;
        return;
    }

    PathHandler::log("[-] Status: Writing the file. [-]");
    if (cv::imwrite(out.string(), img, p)) {
        std::cout << "[+] Saved: " << out.filename() << " [+]" << std::endl;
    }
}

void image() {
    std::string name, fmt;
    std::cout << "Image filename or path: ";
    if(!(std::getline(std::cin >> std::ws, name))) {
        if (std::cin.eof()) {
                std::cout << std::endl;
                PathHandler::log("[-] EOF input received. Exiting gracefully... [-]");
        } else PathHandler::log("[!] Warning: Stream failed or input is illegal. Exiting... [!]");
        PathHandler::log("[~] Status: Clearing input flags. [~]");
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
        if (std::cin.eof()) {
                std::cout << std::endl;
                PathHandler::log("[-] EOF input received. Exiting gracefully... [-]");
        } else PathHandler::log("[!] Warning: Stream failed or input is illegal. Exiting... [!]");
        PathHandler::log("[~] Status: Clearing input flags. [~]");
        std::cin.clear();
        return;
    } // safe lowercase conversion down below
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        PathHandler::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    {    
        bool targetIsRAWImage = (fmt == "cr2" || fmt == "nef" || fmt == "arw" || 
                                fmt == "dng" || fmt == "crw");

        if (targetIsRAWImage) {
            PathHandler::log("[!] Warning: Converting to a RAW image format is not supported, nor recommended! [!]");
            std::cerr << "[!] Error: Detected target is a RAW image format: '" + fmt + "' [!]";
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