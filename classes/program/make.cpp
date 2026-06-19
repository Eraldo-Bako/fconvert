// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "make.hpp"
#include "get.hpp"
#include "../program_handler.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>

std::filesystem::path Program::Make::logFile(const std::string& time) {
    std::string sanitizedTime = time;
    std::replace(sanitizedTime.begin(), sanitizedTime.end(), ' ', '-');
    std::replace(sanitizedTime.begin(), sanitizedTime.end(), ':', '_');

    std::string logFilename = "fconvert-" + sanitizedTime + ".log";
    std::filesystem::path logPath = Program::Get::logDirectory() / logFilename;

    try {
        if (!std::filesystem::exists(logPath)) {
            std::ofstream create_file(logPath);
            
            if (!create_file) {
                throw std::runtime_error("Could not create file stream at " + logPath.string());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[!!] Failed to make the log file: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[!!] Failed to make the log file due to an unknown error." << std::endl;
    }

    static const std::string topLog = R"(
   __                              _   
  / _|                            | |  
 | |_ ___ ___  _ ____   _____ _ __| |_ 
 |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
 | || (_| (_) | | | \ V /  __/ |  | |_ 
 |_| \___\___/|_| |_|\_/ \___|_|   \__\/2.3.0)" "\n" 
R"(
 ----------------------- fconvert - File Converter ----------------------
 ----- A fast CLI converter for Images, Videos, Audios, and Ebooks! -----
 ------------------------------------------------------------------------
    )" "\n";

    std::ofstream logFile(logPath, std::ios::app);
    if (logFile.is_open()) {
        logFile << topLog;
        logFile.close();
    } else {
        std::cerr << "[!] Error: Could not open log file! [!]" << std::endl;
    }

    return logPath;
}

// to do: all input with custom filename and then apply name so shell never executes it
std::filesystem::path Program::Make::tempFile(const std::filesystem::path& original_inputPath) {
    if (!std::filesystem::exists(original_inputPath)) {
        Program::log("[!] Error: File no longer exists: " + original_inputPath.string() + " [!]");
        Program::log("[!] Error: Original file does not exist. Cannot make temp copy. [!]", false);
        return std::filesystem::path();
    }

    Program::log("[~] Status: Creating a secure temporary copy of the input file. [~]");
    
    std::filesystem::path inputDir = original_inputPath.parent_path();
    std::string ext = original_inputPath.extension().string();

    std::filesystem::path tempPath;
    int index = 0;
    do {
        tempPath = inputDir / ("fconvert_tmp_" + std::to_string(index++) + ext);
    } while (std::filesystem::exists(tempPath));

    try {
        std::filesystem::copy_file(original_inputPath, tempPath, std::filesystem::copy_options::overwrite_existing);
        Program::log("[-] Secure temp file created at: " + tempPath.string() + " [-]");
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[!] Error: Failed to create secure temp file: " << e.what() << std::endl;
        return std::filesystem::path();
    }

    return tempPath;
}