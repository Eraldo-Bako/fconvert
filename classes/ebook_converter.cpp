// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "ebook_converter.hpp"
#include "program_handler.hpp"
#include "path_handler.hpp"
#include "secure_conversion_session.hpp"

#include <iostream>
#include <set>
#include <algorithm>
#include <cctype>

bool EbookConverter::convert(const std::filesystem::path& input, const std::string& fmt) {
    std::filesystem::path output = PathHandler::handle_conflicts(PathHandler::get_output_path(input, fmt), false);
    if (output.empty()) return false;

    SecureConversionSession session(input, fmt);

    std::string args = "--standalone --from=markdown+smart";

    if (fmt == "pdf") {
        args += " --variable geometry:margin=1in --variable geometry:a4paper";

        if (!Program::Check::pdfEngine()) {
            Program::print("[!] Error: A supported PDF Engine was not found. [!]\n", Program::PrintType::Error);
            return false;
        }

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

    std::string cmd = Program::Build::command("pandoc", session.safe_input(), session.safe_output(), args);
    Program::log("[-] Status: Executing Ebook Conversion: " + cmd + " [-]");
    if (std::system(cmd.c_str()) == 0) {
        if (session.commit(output)) {
            Program::print("[~] Status: Conversion completed successfully! [~]\n");
        } else {
            Program::print("[!] Error: Failed to safely export output file from sandbox. [!]\n", Program::PrintType::Error);
        }
        return true;
    }
    return false;
}

void ebook_convert_logic(std::filesystem::path in, std::string fmt, bool silent) {
    Program::log("[-] Status: Checking for pandoc... [-]");
    if (!Program::Check::pandoc()) {
        Program::print("[!] Error: Pandoc not found. [!]\n", Program::PrintType::Error);
        return;
    }

    if (!silent) Program::print("[>] Processing document... [>]\n");

    if (EbookConverter::convert(in, fmt)) {
        if (!silent) Program::print("[-] Status: Ebook conversion complete! [-]\n");
    } else {
        Program::print("[!] Error: Pandoc failed to process the document. [!]\n", Program::PrintType::Error);
    }
}

void ebook() {

    std::string name = Program::Get::input("Ebook/Document filename or path: ");
    std::filesystem::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        Program::log("[!] Error: Path could not be resolved. [!]");
        Program::print("[!] File not found. [!]\n");
        return;
    }

    {
        std::string ext = in.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if ( ext == ".pdf") {
            Program::log("[!] Error: PDF files cannot be used as a source format for conversion. [!]");
            Program::print("[!] PDF input is not supported. Pandoc cannot read raw PDF structures. [!]\n");
            return;
        }
    }

    std::string fmt = Program::Get::input("Target Format ([P]df / [H]tml / [E]pub / [T]xt / [D]ocx): ", Program::Case::Lower);
    
    if (fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        Program::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        Program::print("[!] Successfully stopped the conversion! [!]");
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