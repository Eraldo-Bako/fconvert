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
    std::cout << "fconvert (MIT) v2.0.2\n"
              << "Copyright (C) 2026 Eraldo Bako\n"
              << "License MIT\n"
              << "This is free software: you are free to change and redistribute it.\n"
              << "There is NO WARRANTY, to the extent permitted by law.\n"
              << "Written by Eraldo Bako." << std::endl;
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
              << "  eBook:  epub, pdf, html\n"
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
 |_| \___\___/|_| |_|\_/ \___|_|   \__|

 ---------------------------- File Converter ----------------------------
 ----- A fast CLI converter for Images, Videos, Audios, and Ebooks! -----
 ------------------------------------------------------------------------
    )" << std::endl;
}

void interactive_mode() {
    while (true) {
        clear_screen();
        print_banner();
        std::string input;
        std::cout << "Convert [I]mage / [V]ideo / [A]udio / [E]book / [Q]uit: ";
        
        if (!(std::cin >> input)) break;

        for (auto &c : input) c = std::tolower(c);

        if (input == "q" || input == "quit" || input == "exit") {
            break;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (input == "i" || input == "image") {
            image();
        } else if (input == "v" || input == "video") {
            video();
        } else if (input == "a" || input == "audio") {
            audio();
        } else if (input == "e" || input == "ebook" || input == "book" || input == "pdf") {
            std::cout << "[!] Ebook support comming soon! [!]\n";
            continue;
            //ebook();
        } else {
            std::cout << "[!] Invalid input: " << input << " Please try again. [!]\n";
            continue; 
        }

        std::cout << "\nDO you want to convert another file? (y/N): ";
        std::string again;
        std::cin >> again;
        
        if (again.empty() || std::tolower(again[0]) != 'y') {
            break;
        }
        clear_screen();
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    fs::path quick_file;
    std::string quick_ext;

    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "-h" || args[i] == "--help") {
            print_help();
            return 0;
        }
        if (args[i] == "-v" || args[i] == "--version") {
            print_version();
            return 0;
        }
        if (args[i] == "-d" || args[i] == "--debug") {
            PathHandler::debug_mode = true;
        }
        if (args[i] == "-f" && i + 1 < args.size()) {
            quick_file = args[++i];
        }

        if (args[i].size() > 1 && args[i][0] == '-' && 
            args[i] != "-f" && args[i] != "-d" && args[i] != "--debug" && 
            args[i] != "-h" && args[i] != "--help") {
            quick_ext = args[i].substr(1);
        }
    }

    if (!quick_file.empty() && !quick_ext.empty()) {
        fs::path in = PathHandler::resolve_input(quick_file.string());
        if (in.empty()) { 
            std::cerr << "Error: File '" << quick_file.string() << "' not found.\n"; 
            return 1; 
        }
        
        std::string in_ext = in.extension().string();
        std::transform(in_ext.begin(), in_ext.end(), in_ext.begin(), ::tolower);
        std::transform(quick_ext.begin(), quick_ext.end(), quick_ext.begin(), ::tolower);

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
            std::cerr << "Error: Format '-" << quick_ext << "' is not supported.\n";
            return 1;
        }
        return 0;
    }

    interactive_mode();
    return 0;
}