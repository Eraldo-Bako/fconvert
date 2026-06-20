/*
------------------------------------------------------------------------

    fconvert v2.3.0
    A fast CLI converter for Images, Videos, Audios, and Ebooks!
    License: Apache 2.0
    Written by Eraldo Bako.
    Maintainer: eraldobako@gmail.com

       __                              _   
      / _|                            | |  
     | |_ ___ ___  _ ____   _____ _ __| |_ 
     |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
     | || (_| (_) | | | \ V /  __/ |  | |_ 
     |_| \___\___/|_| |_|\_/ \___|_|   \__\/v2.3.0

----------------------- fconvert - File Converter ----------------------
------------------------------------------------------------------------
Copyright 2023-2026 Eraldo Bako

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

------------------------------------------------------------------------
*/

#include "classes/program_handler.hpp"
#include "classes/path_handler.hpp"

#include "classes/image_converter.hpp"
#include "classes/video_converter.hpp"
#include "classes/audio_converter.hpp"
#include "classes/ebook_converter.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

void print_version() {
    std::cout << "fconvert v2.3.0\n"
              << "Copyright (C) 2023-2026 Eraldo Bako\n"
              << "Licensed under the Apache License, Version 2.0 (the \"License\")\n"
              << "\nWritten by Eraldo Bako.\n"
              << "Maintainer: eraldobako@gmail.com" << std::endl;
}

void print_list() {
    std::cout << "Supported Extensions:\n"
              << "  Image: \n"
              << "   STANDARD IMAGES:      jpg/jpeg, png, webp, tiff/tif, bmp\n"
              << "   CAMERA RAW:           cr2, nef, arw, dng, crw\n"
              << "   VECTOR/COMPOSITE:     psd, svg, pdf, gif, ico, xcf, eps, ai\n"
              << "   MODERN COMPRESSED:    heic, heif\n"
              << "  Audio: \n"
              << "   UNCOMPRESSED:         wav, aiff, pcm, dsd\n"
              << "   LOSSLESS COMPRESSED:  flac, alac, wavpack\n"
              << "   LOSSY COMPRESSED:     mp3, ogg, aac, m4a, opus, wma\n"
              << "  Video: \n"
              << "   COMPRESSED/DELIVERY:  mp4, mkv, mov, m4v\n"
              << "   WEB OPTIMIZED:        webm, ogg\n"
              << "   EDITING/INTERMEDIATE: prores, dnxhr, dnxhd\n"
              << "   HARDWARE/ACQUISITION: avchd, mpeg2\n"
              << "   LEGACY:               avi, wmv, flv, f4v, 3gp, 3g2\n"
              << "  eBook(experimental): \n"
              << "   DOCUMENTS:            epub, pdf, html, txt, docx\n\n"
              << "  Note: Inputting a video with an audio extension auto-extracts audio.\n";
}

void print_help() {
    std::cout << "Usage: fconvert [OPTIONS]\n\n"
              << "Options:\n"
              << "  -h, --help        Show this help message\n"
              << "  -l, --list        Show the list of supported formats\n"
              << "  -d, --debug       Enable debug mode (extra logging)\n"
              << "  -f, --file <file-name> -<ext>  Quick Convert: convert <file-name> to target extension <ext>\n\n"
              << "  Note: fconvert accepts absolute or relative paths. If no path is provided, it looks in the sandbox directory.\n\n"
              << "Example: \n"
              << "  fconvert -f /path/to/file.jpg -png\n\n"
              << "Interactive Mode:\n"
              << "  Run fconvert without flags to enter the guided menu.\n\n";
    print_list();
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
    if (Program::debug_mode) std::cout << "/2.3.0";
    std::cout << std::endl << R"(
 ----------------------- fconvert - File Converter ----------------------
 ----- A fast CLI converter for Images, Videos, Audios, and Ebooks! -----
 ------------------------------------------------------------------------
    )";
    Program::log("[~] Detected: Debug mode active. [~]", true, false);
    std::cout << "\n";
}

void interactive_mode() {
    while (true) {
        clear_screen();
        print_banner();
        std::string input = Program::Get::input("Convert [I]mage / [V]ideo / [A]udio / [e]Book / [Q]uit: ", true);

        if (input.empty() || input == "q" || input == "quit" || input == "exit") {
            Program::log("[-] Exit Key Received: " + input + " [-]");
            break;
        }

        if (input == "i" || input == "image") {
            Program::log("[~] Detected: Image key received '" + input + "'. Applying... [~]");
            image();
        } else if (input == "v" || input == "video") {
            Program::log("[~] Detected: Video key received '" + input + "'. Applying... [~]");
            video();
        } else if (input == "a" || input == "audio") {
            Program::log("[~] Detected: Audio key received '" + input + "'. Applying... [~]");
            audio();
        } else if (input == "e" || input == "ebook" || input == "doc" || input == "document") {
            Program::log("[~] Detected: eBook key received '" + input + "'. Applying... [~]");
            Program::print("[!] eBook is currently highly experimental! [!]\n");
            Program::print("[!] Only use it if you know what you are doing. [!]\n");
            ebook();
        } else {
            Program::print("[!] Invalid input: " + input + " Please try again. [!]\n");
            Program::log("[~] Detected: Invalid key received '" + input + "'. Exiting... [~]");
        }

        std::string again = Program::Get::input("\nDo you want to convert another file? (y/N): ", false, false);

        if (std::tolower(static_cast<unsigned char>(again[0])) != 'y') {
            Program::log("[~] Status: Exit key received. Exiting... [~]");
            break;
        } // shouldn't be visible in interactive mode, unless ur pc is really-really-really slow
        Program::log("[~] Status: '" + again.substr(0, 1) + "' received. Restarting... [~]");
    }
}

int main(int argc, char* argv[]) {
    
    std::vector<std::string> args(argv, argv + argc);
    std::filesystem::path quick_file;
    std::string quick_ext;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "-l" || arg == "--list") {
            print_list();
            return 0;
        }else if (arg == "-v" || arg == "--version") {
            print_version();
            return 0;
        }
    }

    const std::string startTime = Program::Get::currentTimestamp();
    Program::activeLogPath = Program::Make::logFile(startTime);
    std::ios_base::sync_with_stdio(false);

    bool file_flag_passed = false;
    
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];

        if (arg == "-d" || arg == "--debug") {
            Program::debug_mode = true;
            Program::log("[~] DEBUG MODE ACTIVE [~]", false);
        }

        if (arg == "-i" || arg == "--intr" || arg == "--interactive") {
            interactive_mode();
            return 0;
        } else if (arg == "-f" || arg == "--file") {
            if (i + 1 >= args.size()) { // didn't find any argument
                Program::print("[!] Error: -f requires a file path argument [!]\n", true);
                return 1;
            }
            std::string next_arg = args[i + 1];
            if (next_arg[0] == '-') { // found flag instead of file path
                Program::print("[!] Error: -f requires a file path, but found a flag: " + next_arg + " [!]\n", true);
                return 1;
            }
            quick_file = args[++i];
            file_flag_passed = true;
            Program::print("[-] File to be converted: " + quick_file.string() + " [-]\n");
            continue;
        } // checks and assigns the desired format
        if (file_flag_passed && arg.size() > 1 && (arg[0] == '-' || arg[0] == '.')) {
            quick_ext = arg.substr(1);
            Program::print("[-] Converting to: " + quick_ext + " [-]\n");
            continue;
        }
    }

    if (!quick_file.empty() && quick_ext.empty()) { // if no extension is provided
        Program::print("[!] Error: -f requires a file extension after file path, but found nothing. [!]\n", true);
        return 1;
    }

    if (!quick_file.empty() && !quick_ext.empty()) { // the -f logic for quick conversion
        std::filesystem::path in = PathHandler::resolve_input(quick_file.string());
        if (in.empty()) { 
            Program::print("[!] Error: File '" + quick_file.string() + "' not found. [!]\n", true);
            return 1; 
        }
        
        std::string in_ext = in.extension().string();
        std::transform(in_ext.begin(), in_ext.end(), in_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::transform(quick_ext.begin(), quick_ext.end(), quick_ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        bool targetIsAudio = (quick_ext == "mp3" || quick_ext == "wav" || quick_ext == "flac" || 
                              quick_ext == "m4a" || quick_ext == "aac" || quick_ext == "ogg" || 
                              quick_ext == "opus" || quick_ext == "wma" || quick_ext == "aiff" ||
                              quick_ext == "pcm" || quick_ext == "dsd" || quick_ext == "alac" || 
                              quick_ext == "wavpack");
        
        bool inputIsVideo  = (in_ext == ".mp4" || in_ext == ".mkv" || in_ext == ".mov" || 
                              in_ext == ".webm" || in_ext == ".avi" || in_ext == ".flv" ||
                              in_ext == ".wmv" || in_ext == ".f4v" || in_ext == ".3gp" || 
                              in_ext == ".3g2" || in_ext == ".m4v" || in_ext == ".f4v"||
                              in_ext == ".mpeg-2" || in_ext == ".avchd" || in_ext == ".mts" ||
                              in_ext == ".m2ts" || in_ext == ".ogv" || in_ext == ".ogg" || 
                              in_ext == ".prores" || in_ext == ".dnxhd" || in_ext == ".dnxhr");

        bool targetIsImage = (quick_ext == "jpg" || quick_ext == "jpeg" || quick_ext == "png" || 
                              quick_ext == "webp" || quick_ext == "tiff" || quick_ext == "tif" || 
                              quick_ext == "bmp" || quick_ext == "gif" || quick_ext == "svg" ||
                              quick_ext == "psd" || quick_ext == "ai" || quick_ext == "xcf" ||
                              quick_ext == "eps" || quick_ext == "ico" || quick_ext == "pdf" ||
                              quick_ext == "heic" || quick_ext == "heif");
        bool targetIsRAWImage = (quick_ext == "cr2" || quick_ext == "nef" || quick_ext == "arw" || 
                                 quick_ext == "dng" || quick_ext == "crw");
        
        bool targetIsDOC = (quick_ext == "epub" || quick_ext == "html" || quick_ext == "pdf" || 
                            quick_ext == "txt" || quick_ext == "docx");

        if (inputIsVideo && targetIsAudio) {
            audio_convert_logic(in, quick_ext, true);
        } else if (targetIsRAWImage) {
            Program::log("[!] Warning: Converting to a RAW image format is not supported, nor recommended! [!]");
            Program::print("[!] Error: Detected target is a RAW image format: '" + quick_ext + "' [!]", true);
            return 1;
        } else if (inputIsVideo) {
            video_convert_logic(in, quick_ext, 'd', true);
        } else if (targetIsAudio) {
            audio_convert_logic(in, quick_ext, true);
        } else if (targetIsImage) {
            image_convert_logic(in, quick_ext, true);
        } else if (targetIsDOC) {
            ebook_convert_logic(in, quick_ext, true);
        } else {
            Program::print("[!] Error: Format '-" + quick_ext + "' is not supported. [!]\n", true);
            return 1;
        }
        return 0;
    }

    interactive_mode();
    return 0;
}