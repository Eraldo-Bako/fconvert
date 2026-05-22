// fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "audio_converter.h"
#include "path_handler.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <set>
#include <sys/wait.h>

void audio_convert_logic(fs::path in, std::string fmt, bool silent) {
    // checking the existence of the input_file and the directory
    // defining the location and handling conflicts for the output_file
    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    // varied on the format, defining the best parameters to fetch to the FFmpeg command
    std::string params;
    PathHandler::log("[~] Status: Defining conversion parameters. [~]");
    if (fmt == "flac" || fmt == "wav") {
        params = "-c:a flac";
    } else if (fmt == "mp3") {
        params = "-c:a libmp3lame -q:a 0";
    } else if (fmt == "ogg") {
        params = "-c:a libvorbis -q:a 5";
    } else if (fmt == "m4a") {
        params = "-c:a aac -b:a 256k -movflags +faststart";
    } else if (fmt == "aac") { // aac is annoying, why do yall even use it
        params = "-fflags +genpts -vn -sn -dn -c:a aac -b:a 256k -af \"aresample=async=1\"";
    } else if (fmt == "opus") { // the best for penguin, use it always
        params = "-c:a libopus -b:a 128k";
    } else {
        params = "-c:a aac -b:a 192k -ar 44100";
    }

    // constructing the FFmpeg command
    std::string cmd = PathHandler::build_ffmpeg_cmd(in.string(), out.string(), params);
    PathHandler::log("[-] Status: Executing Video Conversion: " + cmd + " [-]");
    std::cout << " [~] Status: Converting Audio... [~]" << std::endl;
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

void audio() {
    std::string name, fmt;
    std::cout << "Audio filename or path: "; // safely getting the file name
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
    std::cout << "Format: "; // fetch me the desired format
    if(!(std::getline(std::cin >> std::ws, fmt))) {
        PathHandler::log("[!] Error: No valid input provided! [!]");
        PathHandler::log("[~] Status: Clearing flags and exiting. [~]");
        std::cin.clear();
        return;
    } // safe lowercase conversion down below
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    // why u quitting +@+   jk, just providing a qutitng option at anytime(promise you'll come back Q-Q)
    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        PathHandler::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }
    // seems like you either don't know how to read or write
    static const std::set<std::string> valid_audio = {"mp3", "wav", "flac", "aac", "ogg", "m4a", "wma", "opus"};
    if (valid_audio.find(fmt) == valid_audio.end()) {
        PathHandler::log("[~] Detected: The provided format is incorrect. [~]");
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported audio formats include:\n"
                  << "* Uncompressed: WAV\n"
                  << "* Lossless Compressed: FLAC\n"
                  << "* Lossy Compressed: MP3, AAC, OGG, M4A, WMA, OPUS\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }
    // actual conversion logic
    audio_convert_logic(in, fmt, false);
}