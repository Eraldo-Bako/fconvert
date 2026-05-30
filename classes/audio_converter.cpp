// fconvert v2.1.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "audio_converter.h"
#include "path_handler.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <set>
#ifndef _WIN32
    #include <sys/wait.h>
#endif

void audio_convert_logic(fs::path in, std::string fmt, bool silent) {
    // checking the existence of the input_file and the directory
    // defining the location and handling conflicts for the output_file
    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    // varied on the format, defining the best parameters to fetch to the FFmpeg command
    std::string params;
    PathHandler::log("[~] Status: Defining conversion parameters. [~]");
    // UNCOMPRESSED AUDIO
    if (fmt == "wav") {
        params = "-c:a pcm_s16le";
    } else if (fmt == "aiff") {
        params = "-c:a pcm_s16be";
    } else if (fmt == "pcm") {
        params = "-f s16le -c:a pcm_s16le";
    } else if (fmt == "dsd") {
        params = "-c:a dsd_lsbf_planer"; 
    } // LOSSLESS COMPRESSED AUDIO
    else if (fmt == "flac") {
        params = "-c:a flac";
    } else if (fmt == "alac") {
        params = "-c:a alac";
    } else if (fmt == "wavpack") {
        params = "-c:a wavpack";
    } // LOSSY COMPRESSED AUDIO
    else if (fmt == "mp3") {
        params = "-c:a libmp3lame -q:a 0";
    } else if (fmt == "ogg") {
        params = "-c:a libvorbis -q:a 5";
    } else if (fmt == "m4a") {
        params = "-c:a aac -b:a 256k -movflags +faststart";
    } else if (fmt == "aac") { // aac is annoying, why do yall even use it
        params = "-fflags +genpts -vn -sn -dn -c:a aac -b:a 256k -af \"aresample=async=1\"";
    } else if (fmt == "opus") { // the best for penguin, use it always
        params = "-c:a libopus -b:a 128k";
    } else if (fmt == "wma") { // ms legacy artifact for gramps, gross
        params = "-c:a wmav2 -b:a 192k";
    } else { // --- FALLBACK ---
        params = "-c:a aac -b:a 192k -ar 44100";
    }

    // constructs the FFmpeg command and then executes
    std::string cmd = PathHandler::build_ffmpeg_cmd(in.string(), out.string(), params);
    PathHandler::log("[-] Status: Executing Video Conversion: " + cmd + " [-]");
    std::cout << " [~] Status: Converting Audio... [~]" << std::endl;
    int execute = std::system(cmd.c_str());
    // error catching -_-
    if (execute == -1) {
        // the system shell itself couldn't be started
        std::cerr << "[!] Critical Error: Failed to initiate the command shell. [!]" << std::endl;
    } else { // did the command even finish normally
        #ifdef _WIN32
            int exitCode = execute; 
            if (exitCode == 0) {
                std::cout << "[~] Status: Conversion completed successfully! [~]" << std::endl;
            } else {
                std::cerr << " [!] Error: FFmpeg failed with exit code: " << exitCode << std::endl;
            }
        #else
            if (WIFEXITED(execute)) {
                int exitCode = WEXITSTATUS(execute);
                if (WIFEXITED(execute)) {
                    int exitCode = WEXITSTATUS(execute);
                    if (exitCode == 0) { //successful conversion
                        std::cout << "[~] Status: Conversion completed successfully! [~]" << std::endl;
                    } else { // either the constructed cmd is wrong or FFmpeg is acting up
                        std::cerr << " [!] Error: FFmpeg failed with exit code: " << exitCode << std::endl;
                    }
                } else { // so FFmpeg was terminated either by the user or the system itself(might have crashed)
                    std::cerr << "[!] Error: FFmpeg was terminated abnormally. [!]" << std::endl;
                    std::cout << "[~] If you believe this is a bug, please report it. [~]\n"
                            << "[~] Run fconvert -h or --help for more instructions. [~]" << std::endl;
                }
            }
        #endif
    }
}

void audio() {
    std::string name, fmt;
    std::cout << "Audio filename or path: "; // safely getting the file name
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
        return;
    }
    std::cout << "Format: "; // fetch me the desired format
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
    // why u quitting +@+   jk, just providing a qutitng option at anytime(promise you'll come back Q-Q)
    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        PathHandler::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }
    // seems like you either don't know how to read or write
    static const std::set<std::string> valid_audio = {"mp3", "wav", "flac", "aac", "ogg", "m4a", 
                                                      "wma", "opus", "aiff", "pcm", "dsd", "alac",
                                                      "wavpack"};
    if (valid_audio.find(fmt) == valid_audio.end()) {
        PathHandler::log("[~] Detected: The provided format is incorrect. [~]");
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported audio formats include:\n"
                  << "* Uncompressed: WAV, AIFF, PCM, DSD\n"
                  << "* Lossless Compressed: FLAC, ALAC, WAVPACK\n"
                  << "* Lossy Compressed: MP3, AAC, OGG, M4A, WMA, OPUS\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }
    // actual conversion logic
    audio_convert_logic(in, fmt, false);
}