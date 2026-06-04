// fconvert v2.2.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "ebook_converter.hpp"
#include "path_handler.hpp"

#include <iostream>
#include <cstdlib>
#include <set>
#include <algorithm>
#include <cctype>

bool EbookConverter::check_pandoc() {
#ifdef _WIN32
    return std::system("where pandoc >nul 2>nul") == 0;
#else
    return std::system("command -v pandoc >/dev/null 2>&1") == 0;
#endif
}

bool EbookConverter::convert(const fs::path& input, const std::string& fmt) {
    fs::path output = PathHandler::handle_conflicts(PathHandler::get_output_path(input, fmt), false);
    if (output.empty()) return false;

    std::string args = "--standalone --from=markdown+smart"; 

    if (fmt == "pdf") {
        args += " --variable geometry:margin=1in --variable geometry:a4paper";
        
#ifdef _WIN32
        if (std::system("where typst >nul 2>nul") == 0) {
            args += " --pdf-engine=typst --variable mainfont=\"Times New Roman\"";
        } else if (std::system("where weasyprint >nul 2>nul") == 0) {
            args += " --pdf-engine=weasyprint";
        } else {
            args += " --pdf-engine=xelatex";
        }
#else
        if (std::system("command -v typst >/dev/null 2>&1") == 0) {
            #ifdef __APPLE__
            args += " --pdf-engine=typst --variable mainfont=\"Times New Roman\"";
            #else
            args += " --pdf-engine=typst --variable mainfont=\"Liberation Serif\"";
            #endif
        } else if (std::system("command -v weasyprint >/dev/null 2>&1") == 0) {
            args += " --pdf-engine=weasyprint";
        } else {
            args += " --pdf-engine=xelatex";
        }
#endif
    } else if (fmt == "html") {
        args += " --embed-resources --metadata title=\"fconvert_export\"";
        args += " --variable max-width=40em --variable mathjax=true";
    } else if (fmt == "txt" || fmt == "text" || fmt.empty()) {
        args += " -t plain --wrap=auto";
    } else if (fmt == "docx") {
        args += " --toc --toc-depth=3";
    } else if (fmt == "epub") {
        args += " --split-level=1 --epub-chapter-level=1";
    }

    std::string cmd = PathHandler::build_pandoc_cmd(input.string(), output.string(), args);
    PathHandler::log("[-] Status: Executing Ebook Conversion: " + cmd + " [-]");
    return (std::system(cmd.c_str()) == 0);
}

void ebook_convert_logic(fs::path in, std::string fmt, bool silent) {
    PathHandler::log("[-] Status: Checking for pandoc... [-]");
    if (!EbookConverter::check_pandoc()) {
        std::cerr << " [!] Error: Pandoc not found. [!]\n";
        return;
    }

    if (!silent) std::cout << "[>] Processing document... [>]" << std::endl;

    if (EbookConverter::convert(in, fmt)) {
        if (!silent) std::cout << " [-] Status: Ebook conversion complete! [-]\n";
    } else {
        std::cerr << " [!] Error: Pandoc failed to process the document. [!]\n";
    }
}

void ebook() {
    std::string name, fmt;
    std::cout << "Ebook/Document filename or path: ";
    if(!(std::getline(std::cin >> std::ws, name))) {
        PathHandler::log("[!] Error: No valid input provided! [!]");
        PathHandler::log("[-] Status: Clearing flags and exiting. [-]");
        std::cin.clear();
        return;
    }
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        PathHandler::log("[!] Error: Path could not be resolved. [!]");
        std::cout << " [!] File not found. [!]\n";
        return;
    }

    {
        std::string ext = in.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if ( ext == ".pdf") {
            PathHandler::log("[!] Error: PDF files cannot be used as a source format for conversion. [!]");
            std::cout << " [!] PDF input is not supported. Pandoc cannot read raw PDF structures. [!]\n";
            return;
        }
    }

    std::cout << "Target Format ([P]df / [H]tml / [E]pub / [T]xt / [D]ocx): "; 
    if(!(std::getline(std::cin >> std::ws, fmt))) {
        PathHandler::log("[!] Error: No valid input provided! [!]");
        PathHandler::log("[~] Status: Clearing flags and exiting. [~]");
        std::cin.clear();
        return;
    }
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        PathHandler::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    if (fmt == "p") fmt = "pdf";
    else if (fmt == "h") fmt = "html";
    else if (fmt == "e") fmt = "epub";
    else if (fmt == "t") fmt = "txt";
    else if (fmt == "d") fmt = "docx";

    static const std::set<std::string> valid_ebook = {"pdf", "html", "epub", "txt", "docx"};
    if (valid_ebook.find(fmt) == valid_ebook.end()) {
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported experimental formats include:\n"
                  << "PDF, HTML, EPUB, TXT, DOCX\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }

    ebook_convert_logic(in, fmt, false);
}