// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "get.hpp"
#include "../program_handler.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iostream>
#include <limits>

std::filesystem::path Program::Get::logDirectory() {
    std::filesystem::path logDir;

#if defined(_WIN32)
    const wchar_t* localAppData = _wgetenv(L"LOCALAPPDATA");
    if (localAppData) {
        logDir = std::filesystem::path(localAppData) / "fconvert" / "Logs";
    } else {
        logDir = std::filesystem::current_path();
    }
#else
    const char* xdgState = std::getenv("XDG_STATE_HOME");
    if (xdgState) {
        logDir = std::filesystem::path(xdgState) / "fconvert";
    } else {
        const char* home = std::getenv("HOME");
        if (home) {
            logDir = std::filesystem::path(home) / ".local" / "state" / "fconvert";
        } else {
            logDir = std::filesystem::current_path();
        }
    }
#endif

    if (!logDir.empty() && !std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }

    return logDir;
}

std::string Program::Get::currentTimestamp() {
    time_t timestamp;
    time(&timestamp);
    std::string timeStr = std::ctime(&timestamp);
    if (timeStr.empty()) return "Date/Time Error";

    if (!timeStr.empty() && timeStr.back() == '\n') {
        timeStr.pop_back();
    }

    return timeStr;
}

std::string Program::Get::input(const std::string& prompt, bool lower, bool useWS) {
    if (!prompt.empty()) Program::print(prompt);
    std::string input;
    
    if (!std::cin) {
        Program::log("[!] Error: std::cin is in a bad state prior to reading! [!]");
        return "";
    }

    bool readSuccess = false;
    if (useWS) {
        readSuccess = static_cast<bool>(std::getline(std::cin >> std::ws, input));
    } else {
        readSuccess = static_cast<bool>(std::getline(std::cin, input));
    }

    if (!readSuccess) {
        Program::log("[!] Error: No valid input provided (Stream error or EOF)! [!]");
        Program::log("[~] Status: Clearing flags and recovering stream. [~]");
        
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return "";
    }

    if (input.empty()) {
        Program::log("[!] Error: Input could not be resolved. [!]");
        return "";
    }

    if (lower) { // safe lowercase conversion down below
        Program::log("[-] Status: Converting input to lowercase. [-]");
        std::transform(
            input.begin(), input.end(), input.begin(), [](unsigned char c) { 
                return static_cast<char>(std::tolower(c)); 
            }
        );
    }

    Program::log("[-] Status: Succesfully aquired input: " + input + " [-]");
    return input;
}