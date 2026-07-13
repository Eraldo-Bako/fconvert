// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "program_handler.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

void Program::clearCache() {
    std::error_code ec;
    std::filesystem::path logDir = Program::Get::logDirectory();

    if (std::filesystem::exists(logDir, ec)) {
        for (const auto& entry : std::filesystem::directory_iterator(logDir, ec)) {
            ec.clear();
            std::filesystem::remove_all(entry.path(), ec);
            if (ec)
                Program::print("[!] Failed to delete: " + entry.path().string() + " (" + ec.message() + ")\n", Program::PrintType::Error);
        }
    }
}

void Program::log(const std::string_view msg, const Program::LogDest dest) {

    const bool shouldPrint = (Program::debug_mode && (dest == Program::LogDest::Console || dest == Program::LogDest::All));
    const bool shouldWrite = (dest == Program::LogDest::File || dest == Program::LogDest::All);

    if (shouldPrint) std::clog << Program::Color::CYAN << "[DEBUG] ------ " << msg << Program::Color::RESET << "\n" << std::flush;

    if (shouldWrite) {
        if (std::ofstream logFile(Program::activeLogPath, std::ios::app); logFile.is_open()) {
            logFile << "[" << Program::Get::currentTimestamp() << "]";

            if (Program::debug_mode) logFile << " - [DEBUG] ------ ";
            else logFile << " ------ ";

            logFile << msg << "\n";
        } else {
            std::cerr << Program::Color::RED
                      << "[!] Error: Could not open log file! [!]\n"
                      << Program::Color::RESET << std::flush;
        }
    }
}

// aggressive, using this as a last resort if sth goes rly bad
void Program::end(const std::string& msg) {

    std::cerr << msg << std::endl;
    std::cerr << Program::Color::RED
              << "[!!] Critical Warning: Forcefully ending the program. [!!]\n"
              << Program::Color::RESET << std::flush;

    try {
        Program::log(msg);
    } catch (const std::exception& e) {
        std::cerr << Program::Color::RED
                  << "[!!] Failed to write to log file: " << e.what() << "\n"
                  << Program::Color::RESET << std::flush;
    } catch (...) {
        std::cerr << Program::Color::RED
                  << "[!!] Failed to write to log file due to an unknown error.\n"
                  << Program::Color::RESET << std::flush;
    }
    
    std::fflush(nullptr);
    std::exit(EXIT_FAILURE);

}

void Program::print(const std::string_view msg, PrintType type) {

    if (type != PrintType::Error)
        std::cout << Program::Color::GREEN << msg << Program::Color::RESET << std::flush;
    else {
        std::cout << std::flush;
        std::cerr << Program::Color::RED << msg << Program::Color::RESET << std::flush;
    }

    std::string logMsg = (type != PrintType::Error) ? "[fconvert] - " : "[fconvert Error] - ";
    logMsg.append(msg);

    Program::log(logMsg, Program::LogDest::File);

}