// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "program_handler.hpp"
#include "path_handler.hpp"

#include <iostream>
#include <set>
#include <fmt/core.h>

std::filesystem::path PathHandler::resolve_input(const std::string& input_filename) {

    std::string filename = input_filename;

    // trims leading whitespace
    auto start = filename.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) filename = "";
    else {
        auto end = filename.find_last_not_of(" \t\r\n");
        filename = filename.substr(start, end - start + 1);
    }

    // remove single or double quotes added by terminal drag n`drop
    if (filename.size() >= 2) {
        char front = filename.front();
        char back = filename.back();
        if ((front == '\'' && back == '\'') || (front == '"' && back == '"'))
            filename = filename.substr(1, filename.size() - 2);
    }

    // self-explanatory, read the Program::log calls if needed
    Program::log(fmt::format(_("[-] Searching for the file: {0} [-]"), filename));
    std::filesystem::path p(filename);
    
    Program::log(fmt::format(_("[-] Checking: Is {0} an absolute path? [-]"), filename));
    if (p.is_absolute() && std::filesystem::exists(p)) {
        Program::log(fmt::format(_("[-] Result: {0} is an absolute path. [-]"), filename));
        Program::log(_("[-] Proceeding... [-]"));
        return p;
    } else Program::log(_("[-] Result: Input is NOT an absolute path. [-]"));

    std::filesystem::path local = std::filesystem::current_path() / filename;
    Program::log(fmt::format(_("[-] Checking: Is {0} in the current working directory? [-]"), filename));
    if (std::filesystem::exists(local)) {
        Program::log(fmt::format(_("[-] Result: {0} is in the current working directory. [-]"), filename));
        Program::log(_("[-] Proceeding... [-]"));
        return local;
    } else Program::log(fmt::format(_("[-] Result: {0} is NOT in the current local directory. [-]"), filename));

    std::filesystem::path sandbox = std::filesystem::current_path() / "input_file" / filename;
    Program::log(fmt::format(_("[-] Checking: Is {0} in the sandbox directory? [-]"), filename));
    if (std::filesystem::exists(sandbox)) {
        Program::log(fmt::format(_("[-] Result: {0} is in the sandbox directory. [-]"), filename));
        Program::log(_("[-] Proceeding... [-]"));
        return sandbox;
    } else Program::log(fmt::format(_("[-] Result: {0} is NOT in the sandbox directory. [-]"), filename));

    Program::log(_("[!] Warning: Checks failed to find the file. [!]"));
    Program::print(
        fmt::format(_("\n[!] Error: Could not find '{0}' in the absolute, working, or sandbox directory. [!]\n"), filename), 
        Program::PrintType::Error
    );
    Program::log(_("[!] Warning: Applying default, if any... [!]"));
    Program::log(_("[~] Status: This may fail. [~]"));
    return {}; // trusting the user
}

std::filesystem::path PathHandler::get_output_path(const std::filesystem::path& inputPath, const std::string& extension) {

    Program::log(_("[~] Status: Defining the output directory. [~]"));
    const std::filesystem::path absoluteInput = std::filesystem::absolute(inputPath);
    const std::filesystem::path inputDir = absoluteInput.parent_path();
    std::filesystem::path outDir;

    if (inputDir.filename() == "input_file") { // for sandbox conversion
        outDir = inputDir.parent_path() / "output_file";
    } else {
        outDir = inputDir; // the output will be provided in the same directory as the input
    }
    Program::log(fmt::format(_("[-] Result: Output directory set to '{0}' [-]"), outDir.string()));

    if (!outDir.empty() && !std::filesystem::exists(outDir)) {
        Program::log(fmt::format(_("[!] Warning: Output directory '{0}' does NOT exist. [!]"), outDir.string()));
        Program::log(_("[~] Status: Creating the Directory! [~]"));
        try {
            std::filesystem::create_directories(outDir);
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << fmt::format(_("[!] Permission denied: Could not create '{0}' [!]\n"), outDir.string())
                      << _("[!] Warning: Check your folder permissions or run with appropriate privileges. [!]") << std::endl;
            Program::log(fmt::format(_("[!] Error: Could not create directory '{0}' [!]"), outDir.string()));
            return {};
        }
        if(std::filesystem::exists(outDir)) 
            Program::log(fmt::format(_("[-] Result: Successfully created '{0}' [-]"), outDir.string()));
    }

    std::string formattedExt;
    if (extension.empty()) {
        Program::log(_("[!] Critical Error: Extension is empty. [!]"));
        Program::log(_("[!] Warning: Program may be corrupt. Try restarting or reinstalling if this issue persists. [!]"));
        throw std::runtime_error(_("[!] Critical Error: Empty extension provided to PathHandler. Internal error."));
    }
    if (!extension.empty() && extension[0] == '.') {
        formattedExt = extension;
        Program::log(_("[~] Detected: Extension already formatted. [~]"));
    } else {
        formattedExt = "." + extension; // impossible case, reporting as an error catcher
        Program::log(_("[~] Status: Prepending dot to extension: ") + formattedExt);
    }
    std::filesystem::path out = outDir / absoluteInput.stem(); // full path + file name
    out.replace_extension(formattedExt); // appending extension
    Program::log(_("[-] Result: Successfully defined output file directory and extension as: ") + out.string());
    return out;
}

std::filesystem::path PathHandler::handle_conflicts(std::filesystem::path target, bool silent) {

    Program::log(_("[~] Status: Handling naming conflicts. [~]"));
    if (!std::filesystem::exists(target)) {
        Program::log(_("[-] Result: PATH is available, no conflicts found. Skipping. [-]"));
        return target;
    }
    
    if (!silent) {
        Program::log(fmt::format(_("[!] Warning: File '{0}' already exists, defaulting to conflict handler. [!]"), target.string()));
        std::string c = Program::Get::input(fmt::format(_("[!] Conflict: '{0}' exists. [O]verwrite, [R]ename, [C]ancel: "), target.filename().string()), Program::Case::Lower);
        Program::log(fmt::format(_("[~] Status: Option '{0}' chosen. Applying... [~]"), c));
        if (c[0] == 'o') return target;
        if (c[0] == 'c') return "";
    }
    // program defaults to renaming for safety
    Program::log(_("[~] Status: Initializing renaming logic. [~]"));
    int i = 1; // index for renaming file(1).mp3
    Program::log(_("[~] Status: Acquiring full directory. [~]"));
    const std::filesystem::path baseDir = target.parent_path();
    Program::log(_("[~] Status: Acquiring file name. [~]"));
    const std::string stem = target.stem().string();
    Program::log(_("[~] Status: Acquiring extension. [~]"));
    const std::string ext = target.extension().string();
    
    Program::log(_("[~] Status: Checking for available indexes. [~]"));
    std::filesystem::path newPath = target;
    while (std::filesystem::exists(newPath)) { // testing indexes until one works without overwriting any existing file
        Program::log(fmt::format(_("[~] Status: Testing index: '{0}' [~]"), std::to_string(i)));
        newPath = baseDir / (stem + " (" + std::to_string(i++) + ")" + ext);
    }
    Program::log(fmt::format(_("[-] Result: Index '{0}' succeeded. [-]"), std::to_string(i - 1)));
    Program::log(fmt::format(_("[~] Status: Saving as: {0} [~]"), newPath.string()));
    return newPath;
}