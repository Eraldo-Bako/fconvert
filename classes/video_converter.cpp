// fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "video_converter.h"
#include "path_handler.h"
#include <iostream>
#include <algorithm>
#include <set>

void video_convert_logic(fs::path in, std::string fmt, char q, bool silent) {

    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    std::string params;
    if (fmt == "webm") {
        if (q == 'b')      params = "-c:v libvpx-vp9 -crf 20 -b:v 0 -deadline best -c:a libopus";
        else if (q == 'q') params = "-c:v libvpx-vp9 -crf 35 -b:v 0 -deadline realtime -c:a libopus";
        else               params = "-c:v libvpx-vp9 -crf 30 -b:v 0 -deadline good -c:a libopus";
    } else {
        if (q == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a aac -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a aac -b:a 160k";
    }

    std::string cmd = PathHandler::build_ffmpeg_cmd(in.string(), out.string(), params);
    PathHandler::log("[-] Status: Executing Video Conversion: " + cmd + " [-]");
    std::cout << "[~] Status: Converting Video... [~]" << std::endl;
    int execute = std::system(cmd.c_str());
    // error catching -_-
    if (execute == -1) {
        // the system shell itself couldn't be started
        std::cerr << "[!] Critical Error: Failed to initiate the command shell. [!]" << std::endl;
    } else {    // did the command even finish normally
        if (WIFEXITED(execute)) {
            int exitCode = WEXITSTATUS(execute);
            if (exitCode == 0) { //successful conversion
                std::cout << "[~] Status: Conversion completed successfully! [~]" << std::endl;
            } else {    // either the constructed cmd is wrong or FFmpeg is acting up
                std::cerr << " [!] Error: FFmpeg failed with exit code: " << exitCode << std::endl;
            }
        } else {    // so FFmpeg was terminated either by the user or the system itself(might have crashed)
            std::cerr << "[!] Error: FFmpeg was terminated abnormally. [!]" << std::endl;
            std::cout << "[~] If you believe this is a bug, please report it. [~]\n"
                      << "[~] Run fconvert -h or --help for more instructions. [~]" << std::endl;
        }
    }
}

void video() {
    std::string name, fmt, qual;
    std::cout << "Video filename: ";
    if(!(std::getline(std::cin >> std::ws, name))) {
        PathHandler::log("[!] Error: No valid input provided! [!]");
        PathHandler::log("[~] Status: Clearing flags and exiting. [~]");
        std::cin.clear();
        return;
    }
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        PathHandler::log("[!] Error: Path could not be resolved. [!]");
        return;
    }

    std::cout << "Format: ";
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

    static const std::set<std::string> valid_video = {"mp4", "mov", "avi", "wmv", "flv", "f4v", "mkv", "webm", "3gp", "3g2", "m4v"};
    if (valid_video.find(fmt) == valid_video.end()) {
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported video formats include:\n"
                  << "MP4 (H.264), MOV, AVI, WMV, FLV & F4V, MKV, WebM, 3GP & 3G2, M4V\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }

    std::cout << "Select Quality ([B]est, [G]ood, [Q]uick): "; if (!(std::cin >> qual)) return;
    if (qual == "quit" || qual == "exit" || qual == "cancel") {
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    video_convert_logic(in, fmt, std::tolower(qual[0]), false);
}