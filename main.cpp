/*
------------------------------------------------------------------------
    fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
    A fast CLI converter for Images, Videos, Audios, and Ebooks
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Written by Eraldo Bako.
    Maintaier: eraldobako@gmail.com
       __                              _   
      / _|                            | |  
     | |_ ___ ___  _ ____   _____ _ __| |_ 
     |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
     | || (_| (_) | | | \ V /  __/ |  | |_ 
     |_| \___\___/|_| |_|\_/ \___|_|   \__|
----------------------- fconvert - File Converter ----------------------
------------------------------------------------------------------------
*/
#include "classes/image_converter.h"
#include "classes/video_converter.h"
#include "classes/audio_converter.h"
#include "classes/ebook_converter.h"
#include "classes/path_handler.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <limits>

namespace fs = std::filesystem;

void print_version() {
    std::cout << "fconvert (MIT) v2.0.3\n"
              << "Copyright (C) 2023 - 2026 Eraldo Bako\n"
              << "License MIT\n"
              << "This is free software: you are free to change and redistribute it.\n"
              << "There is NO WARRANTY, to the extent permitted by law.\n"
              << "\nWritten by Eraldo Bako.\n"
              << "Maintaier: eraldobako@gmail.com" << std::endl;
}

void print_help() {
    std::cout << "Usage: fconvert [options]\n\n"
              << "Options:\n"
              << "  -h, --help        Show this help message\n"
              << "  -d, --debug       Enable debug mode (extra logging)\n"
              << "  -f <file> -<ext>  Quick Convert: convert <file> to target <ext>\n\n"
              << "Interactive Mode:\n"
              << "  Run fconvert without flags to enter the guided menu.\n\n"
              << "Supported Extensions:\n"
              << "  Image: jpg, jpeg, png, webp, tiff, bmp\n"
              << "  Audio:  mp3, wav, flac, ogg, m4a, opus\n"
              << "  Video:  mp4, mkv, mov, webm, avi, flv\n"
              << "  eBook:  epub, pdf, html, txt\n"
              << "  Note:   Inputting a video with an audio extension auto-extracts audio.\n";
}

void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[3J\033[H\033[2J" << std::flush;
#endif
}

void print_banner() {
    std::cout << R"(
   __                              _   
  / _|                            | |  
 | |_ ___ ___  _ ____   _____ _ __| |_ 
 |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
 | || (_| (_) | | | \ V /  __/ |  | |_ 
 |_| \___\___/|_| |_|\_/ \___|_|   \__\)";
    if (PathHandler::debug_mode) std::cout << "/2.0.3" << std::endl;
    std::cout << R"(
 ---------------------------- File Converter ----------------------------
 ----- A fast CLI converter for Images, Videos, Audios, and Ebooks! -----
 ------------------------------------------------------------------------
    )";
    PathHandler::log("[~] Detected: Debug mode active. [~]");
    std::cout << std::endl;
}

void interactive_mode() {
    while (true) {
        clear_screen();
        print_banner();
        std::string input;
        std::cout << "Convert [I]mage / [V]ideo / [A]udio ";
        if (PathHandler::debug_mode) std::cout << "/ [E]book ";
        std::cout << "/ [Q]uit: ";
        
        if (!std::getline(std::cin >> std::ws, input)) {
            PathHandler::log("[!] Warning: No input received or input is illegal. Exiting... [!]");
            PathHandler::log("[~] Status: Clearing potential input errors. [~]");
            std::cin.clear();
            break;
        }
        // for (auto &c : input) c = std::tolower(c);
        std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        if (input.empty() || input == "q" || input == "quit" || input == "exit") {
            PathHandler::log("[-] Exit Key Received: " + input + " [-]");
            break;
        }

        if (input == "i" || input == "image") {
            PathHandler::log("[~] Detected: Image key received '" + input + "'. Applying... [~]");
            image();
        } else if (input == "v" || input == "video") {
            PathHandler::log("[~] Detected: Video key received '" + input + "'. Applying... [~]");
            video();
        } else if (input == "a" || input == "audio") {
            PathHandler::log("[~] Detected: Audio key received '" + input + "'. Applying... [~]");
            audio();
        } else if (input == "e" || input == "ebook" || input == "book" || input == "pdf") {
            PathHandler::log("[~] Detected: Ebook key received '" + input + "'. Applying... [~]");
            std::cout << "[!] Ebook is currently highly experimental! [!]\n";
            std::cout << "[!] Only use it if you know what you are doing. [!]\n";
            ebook();
        } else {
            std::cout << "[!] Invalid input: " << input << " Please try again. [!]\n";
            PathHandler::log("[~] Detected: Invalid key received '" + input + "'. Exiting... [~]");
        }

        std::cout << "\nDo you want to convert another file? (y/N): ";
        std::string again;
        if(!(std::getline(std::cin, again))) {
            PathHandler::log("[!] Warning: Stream failed or input is illegal. Exiting... [!]");
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
        if (again.empty()) {
            PathHandler::log("[!] No input received. Exiting... [!]");
            break;
        }
        if (std::tolower(static_cast<unsigned char>(again[0])) != 'y') {
            PathHandler::log("[~] Status: Exit key received. Exiting... [~]");
            break;
        }
        PathHandler::log("[~] Status: '" + again.substr(0, 1) + "' received. Restarting... [~]"); //shouldn't be visible in interactive mode
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    fs::path quick_file;
    std::string quick_ext;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        }
        if (arg == "-v" || arg == "--version") {
            print_version();
            return 0;
        }
        if (arg == "-d" || arg == "--debug") {
            PathHandler::debug_mode = true;
            continue; 
        }
        if (arg == "-f") {
            if (i + 1 >= args.size()) { // didn't find any argument
                std::cerr << "[!] Error: -f requires a file path argument [!]" << std::endl;
                return 1;
            }
            std::string next_arg = args[i + 1];
            if (next_arg[0] == '-') { // found flag instead of file path
                std::cerr << "[!] Error: -f requires a file path, but found a flag: " << next_arg << " [!]" << std::endl;
                return 1;
            }
            quick_file = args[++i];
            std::cout << quick_file << std::endl;
            continue;
        } // checks and assigns the desired format
        if (arg.size() > 1 && (arg[0] == '-' || arg[0] == '.')) {
            quick_ext = arg.substr(1);
            std::cout << quick_ext << std::endl;
            continue;
        }
    }

    if (!quick_file.empty() && quick_ext.empty()) { // if no extension is provided
        std::cerr << "[!] Error: -f requires a file extension after file path, but found nothing. [!]" << std::endl;
        return 1;
    }

    if (!quick_file.empty() && !quick_ext.empty()) { // the -f logic for quick conversion
        fs::path in = PathHandler::resolve_input(quick_file.string());
        if (in.empty()) { 
            std::cerr << "[!] Error: File '" << quick_file.string() << "' not found. [!]\n"; 
            return 1; 
        }
        
        std::string in_ext = in.extension().string();
        std::transform(in_ext.begin(), in_ext.end(), in_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::transform(quick_ext.begin(), quick_ext.end(), quick_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        bool targetIsAudio = (quick_ext == "mp3" || quick_ext == "wav" || quick_ext == "flac" || 
                              quick_ext == "m4a" || quick_ext == "aac" || quick_ext == "ogg" || 
                              quick_ext == "opus");
        
        bool inputIsVideo  = (in_ext == ".mp4" || in_ext == ".mkv" || in_ext == ".mov" || 
                              in_ext == ".webm" || in_ext == ".avi" || in_ext == ".flv");

        bool targetIsImage = (quick_ext == "jpg" || quick_ext == "jpeg" || quick_ext == "png" || 
                              quick_ext == "webp" || quick_ext == "tiff" || quick_ext == "bmp");

        if (inputIsVideo && targetIsAudio) {
            audio_convert_logic(in, quick_ext, true);
        } 
        else if (inputIsVideo) {
            video_convert_logic(in, quick_ext, 'g', true);
        } 
        else if (targetIsAudio) {
            audio_convert_logic(in, quick_ext, true);
        } 
        else if (targetIsImage) {
            image_convert_logic(in, quick_ext, true);
        }
        else {
            std::cerr << "[!] Error: Format '-" << quick_ext << "' is not supported. [!]\n";
            return 1;
        }
        return 0;
    }

    interactive_mode();
    return 0;
}