// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "program_handler.hpp"
#include "path_handler.hpp"

#include <iostream>
#include <set>
#include <cctype>
#include <limits>

std::filesystem::path PathHandler::resolve_input(const std::string& filename) {
    // self explonatory, read the Program::log calls if needed
    Program::log("[-] Searching for the file: " + filename + " [-]");
    std::filesystem::path p(filename);
    
    Program::log("[-] Checking: Is " + filename + " an absolute path? [-]");
    if (p.is_absolute() && std::filesystem::exists(p)) {
        Program::log("[-] Result: " + filename + " is an absolute path. [-]");
        Program::log("[-] Proceeding... [-]");
        return p;
    } else Program::log("[-] Result: Input is NOT an absolute path. [-]");

    std::filesystem::path local = std::filesystem::current_path() / filename;
    Program::log("[-] Checking: Is " + filename + " in the current working directory? [-]");
    if (std::filesystem::exists(local)) {
        Program::log("[-] Result: " + filename + " is in the current working directory. [-]");
        Program::log("[-] Proceeding... [-]");
        return local;
    } else Program::log("[-] Result: " + filename + " is NOT in the current local directory. [-]");

    std::filesystem::path sandbox = std::filesystem::current_path() / "input_file" / filename;
    Program::log("[-] Checking: Is " + filename + " in the sandbox directory? [-]");
    if (std::filesystem::exists(sandbox)) {
        Program::log("[-] Result: " + filename + " is in the sandbox directory. [-]");
        Program::log("[-] Proceeding... [-]");
        return sandbox;
    } else Program::log("[-] Result: " + filename + " is NOT in the sandbox directory. [-]");

    Program::log("[!] Warning: Checks failed to find the file. [!]");
    std::cerr << "[!] Error: Could not find '" << filename << "' in the absolute, working, or sandbox directory. [!]" << std::endl;
    Program::log("[!] Warning: Applying default, if any... [!]");
    Program::log("[~] Status: This may fail. [~]");
    return p; // trusting the user
}

std::filesystem::path PathHandler::get_output_path(const std::filesystem::path& inputPath, const std::string& extension) {

    Program::log("[~] Status: Defining the output directory. [~]");
    std::filesystem::path absoluteInput = std::filesystem::absolute(inputPath);
    std::filesystem::path inputDir = absoluteInput.parent_path();
    std::filesystem::path outDir;

    if (inputDir.filename() == "input_file") { // for sandbox conversion
        outDir = inputDir.parent_path() / "output_file";
    } else {
        outDir = inputDir; // the output will be provided in the same directory as the input
    }
    Program::log("[-] Result: Output directory set to " + outDir.string() + "[-]");

    if (!outDir.empty() && !std::filesystem::exists(outDir)) {
        Program::log("[!] Warning: " + outDir.string() + " does NOT exist. [!]");
        Program::log("[~] Status: Creating the Directory! [~]");
        try {
            std::filesystem::create_directories(outDir);
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[!] Permission denied: Could not create " << outDir.string() << " [!]\n"
                      << "[!] Warning: Check your folder permissions or run with appropriate privileges. [!]" << std::endl;
            Program::log("[!] Error: Could not create directory " + outDir.string() + "[!]");
            return std::filesystem::path();
        }
        if(std::filesystem::exists(outDir)) Program::log("[-] Result: Sucessfully created " + outDir.string() + " [-]");
    }

    std::string formattedExt;
    if (extension.empty()) {
        Program::log("[!] Critical Error: Extension is empty. [!]");
        Program::log("[!] Warning: Programm may be corrupt. Try restarting or reinstalling if this issue presists. [!]");
        throw std::runtime_error("[!] Critical Error: Empty extension provided to PathHandler. Internal error. ");
    }
    if (!extension.empty() && extension[0] == '.') {
        formattedExt = extension;
        Program::log("[~] Detected: Extension already formatted. [~]");
    } else {
        formattedExt = "." + extension; // impossible case, repporting as a error catcher
        Program::log("[~] Status: Prepending dot to extension: " + formattedExt);
    }
    std::filesystem::path out = outDir / absoluteInput.stem(); // full path + file name
    out.replace_extension(formattedExt); // appending extension
    Program::log("[-] Result: Sucessfully defined output file directory and extension as: " + out.string());
    return out;
}

std::filesystem::path PathHandler::handle_conflicts(std::filesystem::path target, bool silent) {

    Program::log("[~] Status: Handling naming conflicts. [~]");
    if (!std::filesystem::exists(target)) {
        Program::log("[-] Result: PATH is available, no conflicts found. Skipping. [-]");
        return target;
    }
    
    if (!silent) {
        Program::log("[!] Warning: File '" + target.string() + "' already exists, defaulting to conflict handler. [!]");
        std::cout << " [!] Conflict: " << target.filename().string() << " exists. (o)verwrite, (r)ename, (c)ancel: ";
        char c; 
        if (!(std::cin >> std::ws >> c)) {
            Program::log("[!] Error: No input received. [!]");
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        c = std::tolower(static_cast<unsigned char>(c));
        Program::log("[~] Status: Option '" + std::string(1, c) + "' chosen. Applying... [~]");
        if (c == 'o') return target;
        if (c == 'c') return "";
    }
    // program defaults to renaming for safety
    Program::log("[~] Status: Initializing renaming logic. [~]");
    int i = 1; // index for renaminf file(1).mp3
    Program::log("[~] Status: Acquiring full directory. [~]");
    std::filesystem::path baseDir = target.parent_path();
    Program::log("[~] Status: Acquiring file name. [~]");
    std::string stem = target.stem().string();
    Program::log("[~] Status: Acquiring extension. [~]");
    std::string ext = target.extension().string();
    
    Program::log("[~] Status: Checking for available indexes. [~]");
    std::filesystem::path newPath = target;
    while (std::filesystem::exists(newPath)) { // testing indexes until one works without overwriting any existing file
        Program::log("[~] Status: Testing index: " + std::to_string(i) + " [~]");
        newPath = baseDir / (stem + " (" + std::to_string(i++) + ")" + ext);
    }
    Program::log("[-] Result: Index '" + std::to_string(i - 1) + "' succeeded. [-]");
    Program::log("[~] Status: Saving as: " + newPath.string() + " [~]");
    return newPath;
}