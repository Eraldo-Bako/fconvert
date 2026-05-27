// fconvert v2.1.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "ebook_converter.h"
#include "path_handler.h"
#include <cstdlib>
#include <set>
#include <algorithm>
#include <cctype>
#include <limits>
#include <ios>

bool EbookConverter::check_pandoc() {
#ifdef _WIN32
    return std::system("where pandoc >nul 2>nul") == 0;
#else
    return std::system("command -v pandoc >/dev/null 2>&1") == 0;
#endif
}

bool EbookConverter::convert(const fs::path& input, const std::string& target_ext) {
    fs::path output = PathHandler::handle_conflicts(PathHandler::get_output_path(input, target_ext), false);
    if (output.empty()) return false;

    std::string args = "--standalone"; 

    if (target_ext == "pdf") {
        args += " --variable margin-top=1 --variable margin-bottom=1";
        
        if (std::system("command -v weasyprint >/dev/null 2>&1") == 0) {
            args += " --pdf-engine=weasyprint";
        }
    } else if (target_ext == "html") {
        args += " --embed-resources --metadata title=\"fconvert_export\"";
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