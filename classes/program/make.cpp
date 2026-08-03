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
                throw std::runtime_error(fmt::format(_("Could not create file stream at {0}"), logPath.string()));
            }
        }
    } catch (const std::exception& e) {
        std::cerr << Program::Color::RED
                  << fmt::format(_("[!!] Failed to make the log file: {0} [!!]"), e.what())
                  << Program::Color::RESET << std::endl << std::flush;
    } catch (...) {
        std::cerr << Program::Color::RED
                  << _("[!!] Failed to make the log file due to an unknown error.")
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
                  << _("[!] Error: Could not open log file after creation! [!]")
                  << Program::Color::RESET << std::endl <<std::flush;
    }

    return logPath;
}
