/*
------------------------------------------------------------------------

    fconvert v2.4.1
    A fast C++ CLI converter for Images, Videos, Audios, and Ebooks!
    License: Apache 2.0
    Written by Eraldo Bako.
    Maintainer: eraldobako@gmail.com

       __                              _   
      / _|                            | |  
     | |_ ___ ___  _ ____   _____ _ __| |_ 
     |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
     | || (_| (_) | | | \ V /  __/ |  | |_ 
     |_| \___\___/|_| |_|\_/ \___|_|   \__\/v2.4.1

----------------------- fconvert - File Converter ----------------------
------------------------------------------------------------------------
Copyright 2023-2026 Eraldo Bako

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

------------------------------------------------------------------------
*/

///////////////////////////////////////////////////////////////
// The official info for this program.
#define PROGRAM_NAME "fconvert"
#define PROGRAM_AUTHOR "Eraldo Bako"
#define PROGRAM_AUTHOR_EMAIL "eraldobako@gmail.com"
#define PROGRAM_LICENSE "Apache v2.0, the License"

extern "C" [[gnu::used, gnu::section(".metadata")]] inline const char METADATA_BANNER[] = 
    "=== " PROGRAM_NAME " | " PROGRAM_AUTHOR " " PROGRAM_AUTHOR_EMAIL " | " PROGRAM_LICENSE " ===";

///////////////////////////////////////////////////////////////

#include "classes/program_handler.hpp"
#include "classes/path_handler.hpp"

#include "classes/image_converter.hpp"
#include "classes/video_converter.hpp"
#include "classes/audio_converter.hpp"
#include "classes/ebook_converter.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cctype>

void print_version(const char* program_name = PROGRAM_NAME) {
    std::cout << program_name << " v2.4.1\n"
              << fmt::format(_("Copyright (C) 2023-2026 {0}\n"), PROGRAM_AUTHOR)
              << _("Licensed under the Apache License, Version 2.0 (the \"License\")\n")
              << fmt::format(_("Read a copy of the license at {0}\n\n"), "<https://www.apache.org/licenses/LICENSE-2.0>")
              << fmt::format(_("This is free software: you are free to change and redistribute {0}it in accordance to the terms of the LICENSE.\n"), "\n")
              << _("There is NO WARRANTY, to the extent permitted by law.\n\n")
              << fmt::format(_("Written by {0}.\n"), PROGRAM_AUTHOR)
              << fmt::format( _("Maintainer: {0}\n"), PROGRAM_AUTHOR_EMAIL)
              << _("Official Repository: https://github.com/Eraldo-Bako/fconvert") << std::endl;
}

void print_list() {
    std::cout << _("Supported Extensions:\n")
              << _("  Image: \n")
              << _("   STANDARD IMAGES:                jpg/jpeg, png, webp, tiff/tif, bmp\n")
              << _("   CAMERA RAW:                     cr2, nef, arw, dng, crw\n")
              << _("   VECTOR/COMPOSITE:               psd, svg, pdf, gif, ico, xcf, eps, ai\n")
              << _("   MODERN COMPRESSED:              heic, heif\n")
              << _("  Audio: \n")
              << _("   UNCOMPRESSED:                   wav, aiff, pcm, dsd\n")
              << _("   LOSSLESS COMPRESSED:            flac, alac, wavpack\n")
              << _("   LOSSY COMPRESSED:               mp3, ogg, aac, m4a, opus, wma\n")
              << _("  Video: \n")
              << _("   COMPRESSED/DELIVERY:            mp4, mkv, mov, m4v\n")
              << _("   WEB OPTIMIZED:                  webm, ogg\n")
              << _("   EDITING/INTERMEDIATE:           prores, dnxhr, dnxhd\n")
              << _("   HARDWARE/ACQUISITION:           avchd, mpeg2\n")
              << _("   LEGACY:                         avi, wmv, flv, f4v, 3gp, 3g2\n")
              << _("  eBook(experimental): \n")
              << _("   DOCUMENTS:                      epub, pdf, html, txt, docx\n\n")
              << _("  Note: Inputting a video with an audio extension auto-extracts audio.\n\n");
}

void print_help(const char* program_name, int status = EXIT_SUCCESS) {

    if (status != EXIT_SUCCESS) {
        std::cerr << fmt::format(_("Try '{0} --help' for more information.\n"), program_name);
        std::exit(status);
    }

    // Usage and Summary
    std::cout << fmt::format(_("Usage: {0} [OPTIONS]\n"), program_name)
              << fmt::format(_("       {0} -f <file-name> -<ext>\n"), program_name)
              << _("Info: Convert media files to the specified formats.\n\n");

    // Options
    std::cout << _("Options:\n")
              << _("  -v, --version                    Print version and licensing info\n")
              << _("  -h, --help                       Show this help message\n")
              << _("  -l, --list                       Show the list of supported formats\n")
              << _("  -d, --debug                      Enable debug mode (extra logging)\n")
              << _("  -cc, --clear-cache               Clear saved cache and log files\n")
              << _("  -pcd, --print-cache-dir          Print cache and log directory\n")
              << _("  -scd, --specify-cache-dir        Specify cache and log directory\n")
              << _("  -f, --file <file> -<ext>         Convert <file> to target extension <ext>\n\n");

    // Notes & Examples
    std::cout << _("Note:\n")
              << fmt::format(_("  {0} accepts absolute or relative file paths.\n"), program_name)
              << _("If no path is provided, it defaults to searching in the sandbox directory.\n\n")
              << _("Examples:\n")
              << fmt::format(_("  {0} -f /path/to/file.jpg -png\n\n"), program_name)
              << _("Interactive Mode:\n")
              << fmt::format(_("  Run {0} without flags to enter the guided menu.\n\n"), program_name);

    // Dependencies
    std::cout << "\n" << _("Make sure the required external tools are installed and accessible: \n")
              << _("  FFmpeg                           Video and Audio Conversion\n")
              << _("  ImageMagick & Ghostscript        Advanced Image Conversion\n")
              << _("  VTracer                          Advanced Vector Conversion\n")
              << _("  Pandoc                           Document Processing\n")
              << _("  Typst, Weasyprint, or Xelatex    PDF Engine\n\n");
    
    std::cout << "========================================================================\n\n";

    // List of Supported Formats
    print_list();

    std::cout << "========================================================================\n\n";

    // Program Info
    print_version(program_name);
}

void clear_screen() {
    if(WINBLOAT) std::system("cls");
    else std::cout << "\033[3J\033[H\033[2J" << std::flush;
}

void print_banner() {
    std::cout << R"(
   __                              _   
  / _|                            | |  
 | |_ ___ ___  _ ____   _____ _ __| |_ 
 |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
 | || (_| (_) | | | \ V /  __/ |  | |_ 
 |_| \___\___/|_| |_|\_/ \___|_|   \__\)";
    if (Program::debug_mode) std::cout << "/2.4.1" << std::flush;
    std::cout << std::endl << std::flush << _(R"(
 ----------------------- fconvert - File Converter ----------------------
 --- A fast C++ CLI converter for Images, Videos, Audios, and eBooks! ---
 ------------------------------------------------------------------------
    )") << std::flush;
    Program::log(_("[~] Detected: Debug mode active. [~]"), Program::LogDest::Console);
    std::cout << "\n";
}

void interactive_mode() {
    while (true) {
        clear_screen(); print_banner();
        std::string input = Program::Get::input(_("Convert [I]mage / [V]ideo / [A]udio / [e]Book / [Q]uit: "), Program::Case::Lower, Program::InputType::noWS);

        if (input.empty() || input == "q" || input == "quit" || input == "exit") {
            Program::log(fmt::format(_("[-] Exit Key Received: {} [-]"), input));
            break;
        }

        if (input == "i" || input == "img" || input == "image") {
            Program::log(fmt::format(_("[~] Detected: Image key received '{}'. Applying... [~]"), input));
            image();
        } else if (input == "v" || input == "vid" || input == "video") {
            Program::log(fmt::format(_("[~] Detected: Video key received '{}'. Applying... [~]"), input));
            video();
        } else if (input == "a"  || input == "aud"|| input == "audio" || input == "snd" || input == "sound") {
            Program::log(fmt::format(_("[~] Detected: Audio key received '{}'. Applying... [~]"), input));
            audio();
        } else if (input == "e" || input == "ebook" || input == "doc" || input == "document" || input == "pdf") {
            Program::log(fmt::format(_("[~] Detected: eBook key received '{}'. Applying... [~]"), input));
            Program::print(_("[!] eBook is currently highly experimental! [!]\n"));
            Program::print(_("[!] Only use it if you know what you are doing. [!]\n"));
            ebook();
        } else {
            Program::print(fmt::format(_("[!] Invalid input '{0}'. Please try again! [!]\n"), input));
            Program::log(fmt::format(_("[~] Detected: Invalid key received '{0}'. Exiting... [~]"), input));
        }

        std::string again = Program::Get::input(_("\nDo you want to convert another file? (y/N): "), Program::Case::Lower, Program::InputType::noWS);

        if (std::tolower(static_cast<unsigned char>(again[0])) != 'y') {
            Program::log(_("[~] Status: Exit key received. Exiting... [~]"));
            break;
        } // shouldn't be visible in interactive mode, unless ur pc is really-really-rly slow
        Program::log(fmt::format(_("[~] Status: Input '{0}' received. Restarting... [~]"), again.substr(0, 1)));
    }
}

int main(int argc, char* argv[]) {

    std::setlocale(LC_ALL, "");
    std::string loc_dir = Program::Get::localeDirectory();
    bindtextdomain("fconvert", loc_dir.c_str());
    bind_textdomain_codeset("fconvert", "UTF-8");
    textdomain("fconvert");

    {
        if (Program::Check::is_running_as_root()) {
            std::cout << Program::Color::RED
                    << "\n\n=================================================================\n"
                    << fmt::format(_("[CRITICAL WARNING] {0} is running with elevated privileges.\n"), argv[0])
                    << "=================================================================\n\n"
                    << _("Executing file conversions as root bypasses vital OS safeguards.\n")
                    << _("Malformed files or unexpected behavior CAN irreversibly corrupt\n")
                    << _("system binaries or compromise OS integrity.\n\n")
                    << "=================================================================\n\n"
                    << fmt::format(_("If you do not know what you are doing or what is happening,{}"
                                     "abort this operation immediately to prevent startup!\n\n"), "\n")
                    << "=================================================================\n"
                    << Program::Color::RESET;
            std::string confirmation1;
            std::cout << std::flush << fmt::format(_("Type '{0}' to proceed: "), "YES, I ACCEPT THE RISK - PROCEED");
            std::getline(std::cin, confirmation1);

            if (confirmation1 != "YES, I ACCEPT THE RISK - PROCEED") {
                std::cout << Program::Color::YELLOW
                        << _("[~] Operation successfully aborted. [~]\n")
                        << _("[-] Never do that again! [-]\n");
                return 0;
            }

            std::string confirmation2;
            std::cout << Program::Color::YELLOW << _("Are you sure about that?\n")
                    << Program::Color::RESET << fmt::format(_("Type '{0}' to proceed: "), "PROCEED");
            std::getline(std::cin, confirmation2);

            if (confirmation2 != "PROCEED") {
                std::cout << Program::Color::YELLOW
                        << _("[~] Operation successfully aborted. [~]\n")
                        << _("[-] Never do that again! M'KAY? [-]\n");
                return 0;
            }

            std::cout << Program::Color::YELLOW
                    << _("[!] Bootstrapping execution context with root clearance...\n")
                    << _("[-] Status: You are on your own now! [-]\n")
                    << Program::Color::RESET;

        }
    }
    
    std::vector<std::string> args(argv, argv + argc);

    if (args.size() > 1) {
        const std::string& arg = args[1];

        if (arg == "-h" || arg == "--help") {
            print_help(args[0].c_str());
            return EXIT_SUCCESS;
        } else if (arg == "-l" || arg == "--list") {
            print_list();
            return EXIT_SUCCESS;
        }else if (arg == "-v" || arg == "--version") {
            print_version();
            return EXIT_SUCCESS;
        } else if (arg == "-cc" || arg == "--clear-cache") {
            Program::clearCache();
            return EXIT_SUCCESS;
        } else if (arg == "-pcd" || arg == "--print-cache-dir") {
            //TODO
            return EXIT_SUCCESS;
        } else if (arg == "-scd" || arg == "--specify-cache-dir") {
            //TODO
            return EXIT_SUCCESS;
        }
    }

    const std::string startTime = Program::Get::currentTimestamp();
    Program::activeLogPath = Program::Make::logFile(startTime);

    std::cerr.tie(&std::cout);
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::filesystem::path quick_file;
    std::string quick_ext;

    {
        bool FILE_FLAG_PASSED = false;

        for (size_t i = 1; i < args.size(); ++i) {
            const std::string& arg = args[i];

            if (arg == "-d" || arg == "--debug") {
                Program::debug_mode = true;
                Program::log(_("[~] DEBUG MODE ACTIVE [~]"), Program::LogDest::File);
            }

            if (arg == "-i" || arg == "--intr" || arg == "--interactive") {
                interactive_mode();
                return 0;
            } else if (arg == "-f" || arg == "--file") {
                if (i + 1 >= args.size()) { // didn't find any argument
                    Program::print(_("[!] Error: -f requires a file path argument [!]\n"), Program::PrintType::Error);
                    return 1;
                }
                std::string next_arg = args[i + 1];
                if (next_arg[0] == '-') { // found flag instead of file path
                    Program::print(fmt::format(_("[!] Error: -f requires a file path, but found a flag: '{0}' [!]\n"), next_arg), Program::PrintType::Error);
                    return 1;
                }
                quick_file = args[++i];
                FILE_FLAG_PASSED = true;
                Program::print(fmt::format(_("[-] File to be converted: '{0}' [-]\n"), quick_file.string()));
                continue;
            } // checks and assigns the desired format
            if (FILE_FLAG_PASSED && arg.size() > 1 && (arg[0] == '-' || arg[0] == '.')) {
                quick_ext = arg.substr(1);
                Program::print(fmt::format(_("[-] Converting to: '{0}' [-]\n"), quick_ext));
                continue;
            }
        }
    }

    if (!quick_file.empty() && quick_ext.empty()) { // if no extension is provided
        Program::print(_("[!] Error: -f requires a file extension after file path, but found nothing. [!]\n"), Program::PrintType::Error);
        return 1;
    }

    if (!quick_file.empty() && !quick_ext.empty()) { // the -f logic for quick conversion
        std::filesystem::path in = PathHandler::resolve_input(quick_file.string());
        if (in.empty()) { 
            Program::print(fmt::format(_("[!] Error: File '{0}' not found. [!]\n"), quick_file.string()), Program::PrintType::Error);
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
            Program::log(_("[!] Warning: Converting to a RAW image format is not supported, nor recommended! [!]"));
            Program::print(fmt::format(_("[!] Error: Detected target is a RAW image format: '{0}' [!]"), quick_ext), Program::PrintType::Error);
            return EXIT_FAILURE;
        } else if (inputIsVideo) {
            video_convert_logic(in, quick_ext, 'd', true);
        } else if (targetIsAudio) {
            audio_convert_logic(in, quick_ext, true);
        } else if (targetIsImage) {
            bool targetIsVector = (quick_ext == "svg" || quick_ext == "ai");
            if (targetIsVector) {
                Program::log(_("[-] Detected: Converting to a vector-based format! [-]"));
                Program::log(_("[~] Status: Defaulting to color-based vector tracing! [~]"));
                image_convert_logic(in, quick_ext, true, Image::SVG::CVECTOR);
            } else image_convert_logic(in, quick_ext, true);
        } else if (targetIsDOC) {
            ebook_convert_logic(in, quick_ext, true);
        } else {
            Program::print(fmt::format(_("[!] Error: Format '-{}' is not supported. [!]\n"), quick_ext), Program::PrintType::Error);
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    interactive_mode();
    return EXIT_SUCCESS;
}