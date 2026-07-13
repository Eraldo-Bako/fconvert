// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "make.hpp"
#include "get.hpp"
#include "../program_handler.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string_view>

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
        std::cerr << Program::Color::RED
                  << "[!!] Failed to make the log file: " << e.what() << " [!!]"
                  << Program::Color::RESET << std::endl << std::flush;
    } catch (...) {
        std::cerr << Program::Color::RED
                  << "[!!] Failed to make the log file due to an unknown error."
                  << Program::Color::RESET << std::endl << std::flush;
    }

    static const std::string topLog = R"(
   __                              _
  / _|                            | |
 | |_ ___ ___  _ ____   _____ _ __| |_
 |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
 | || (_| (_) | | | \ V /  __/ |  | |_
 |_| \___\___/|_| |_|\_/ \___|_|   \__\/2.4.0)" "\n"
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
        std::cerr << Program::Color::RED
                  << "[!] Error: Could not open log file after creation! [!]"
                  << Program::Color::RESET << std::endl <<std::flush;
    }

    return logPath;
}
/*
std::filesystem::path Program::Make::tempFile(const std::filesystem::path& inputPath, const Program::TempExist operation, const std::filesystem::path& outputPath) {
    if (operation == Program::TempExist::Create) {

        if (!std::filesystem::exists(inputPath)) {
            Program::log("[!] Error: File no longer exists: " + inputPath.string() + " [!]");
            Program::log("[!] Error: Original file does not exist. Cannot make temp copy. [!]", Program::LogDest::File);
            Program::end("[!!! fconvert cannot access the original file !!!]");
            return {}; //shouldn't run
        }

        Program::log("[~] Status: Creating a secure temporary copy of:" + inputPath.string() + " [~]");

        std::filesystem::path inputDir = inputPath.parent_path();
        std::string ext = inputPath.extension().string();

        std::filesystem::path tempPath;
        int index = 0;
        do {
            tempPath = inputDir / ("fconvert_tmp_" + std::to_string(index++) + ext);
        } while (std::filesystem::exists(tempPath));

        try {
            std::filesystem::copy_file(inputPath, tempPath, std::filesystem::copy_options::overwrite_existing);
            Program::log("[-] Secure temp file created at: " + tempPath.string() + " [-]");
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "[!] Error: Failed to create secure temp file: " << e.what() << std::endl;
            Program::end("[!!! fconvert cannot safely convert the file, check logs to see what happend !!!]");
            return {};
        }

        return tempPath;

    } else if (operation == Program::TempExist::Copy) {

        if (!std::filesystem::exists(inputPath)) {
            Program::log("[!] Error: File no longer exists: " + inputPath.string() + " [!]");
            Program::log("[!] Error: Converted file does not exist. Cannot move it. [!]", Program::LogDest::File);
            Program::end("[!!! fconvert cannot access the converted file !!!]");
            return {}; //shouldn't run
        }

        try {
            std::filesystem::copy_file(inputPath, outputPath, std::filesystem::copy_options::overwrite_existing);
            Program::log("[-] Moving converted file created at: " + outputPath.string() + " [-]");
        } catch (const std::filesystem::filesystem_error& e) {
            Program::print("[-] Get your converted file at: <" + inputPath.string() + "> [-]");
            Program::print("[!] Error: Failed to move the converted file: " + std::string(e.what()) + " [-]\n");
            Program::end("[!!! fconvert cannot safely move the converted file, check logs to see what happened !!!]");
            return {};
        }

    } else if (operation == Program::TempExist::Delete) {
        try {
            if (std::filesystem::remove(inputPath) && std::filesystem::remove(outputPath))
                Program::log("Temporary files deleted successfully.\n");
            else Program::log("Nothing to delete, files do not exist.\n");
        } catch (const std::filesystem::filesystem_error& e) {
            Program::log("Error: " + std::string(e.what()) + '\n');
        }
    } else Program::end("[!!! Impossible behavior at 'Program::Make::tempFile' !!!]\n[!!! This should have never happened. !!!]");
    return {};
}*/
