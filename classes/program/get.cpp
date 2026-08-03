// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "get.hpp"
#include "../program_handler.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <limits>

#if defined(_WIN32)
#include <windows.h>

#elif defined(__APPLE__)

#include <mach-o/dyld.h>
#include <climits>

#endif

std::string Program::Get::localeDirectory() {
    const char* appdir = std::getenv("APPDIR");
    if (appdir) return std::string(appdir) + LOCALEDIR;

    try {
        std::filesystem::path exec_path;

#if defined(__linux__)
        exec_path = std::filesystem::canonical("/proc/self/exe");
#elif defined(__APPLE__)
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) exec_path = std::filesystem::canonical(path);
#elif defined(_WIN32)
        wchar_t path[MAX_PATH];
        DWORD length = GetModuleFileNameW(NULL, path, MAX_PATH);
        if (length > 0 && length < MAX_PATH) {
            exec_path = std::filesystem::canonical(path);
        }
#endif

        if (!exec_path.empty()) {
            std::filesystem::path exec_dir = exec_path.parent_path();

            std::filesystem::path build_po = exec_dir / "po";
            if (std::filesystem::exists(build_po)) return build_po.string();

            std::filesystem::path source_build_po = exec_dir.parent_path() / "build" / "po";
            if (std::filesystem::exists(source_build_po)) return source_build_po.string();

            std::filesystem::path relative_loc = exec_dir.parent_path() / "share" / "locale";
            if (std::filesystem::exists(relative_loc)) return relative_loc.string();
        }

    } catch (...) {}

    return (exec_dir / "share" / "locale").string();
}

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

    struct tm timeInfo{};
#ifdef _WIN32
    localtime_s(&timeInfo, &timestamp);
#else
    localtime_r(&timestamp, &timeInfo);
#endif

    char buffer[26];
    size_t len = std::strftime(buffer, sizeof(buffer), "%a %b %d %Y %H:%M:%S", &timeInfo);

    if (len == 0) return "Date/Time Error";

    return std::string(buffer, len);
}

std::string Program::Get::input(const std::string& prompt, Program::Case lower, Program::InputType useWS) {
    if (!prompt.empty()) {
        Program::print(prompt);
    }
    std::string input;
    
    if (!std::cin) {
        Program::log("[!] Error: std::cin is in a bad state prior to reading! [!]");
        return "";
    }

    bool safeInput = (useWS == Program::InputType::WS);
    bool readSuccess = false;
    if (safeInput)
        readSuccess = static_cast<bool>(std::getline(std::cin >> std::ws, input));
    else readSuccess = static_cast<bool>(std::getline(std::cin, input));

    if (!readSuccess) {
        Program::log("[!] Error: No valid input provided (Stream error or EOF)! [!]");
        Program::log("[~] Status: Clearing flags and recovering stream. [~]");
        
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        Program::end("[ Stream reached EOF, ending the program. ]");
    }

    if (input.empty()) {
        Program::log("[!] Error: Input could not be resolved. [!]");
        return "";
    }

    if (lower == Program::Case::Lower) { // safe lowercase conversion down below
        Program::log("[-] Status: Converting input to lowercase. [-]");
        std::transform(
            input.begin(), input.end(), input.begin(), [](unsigned char c) { 
                return static_cast<char>(std::tolower(c)); 
            }
        );
    }

    Program::log("[-] Status: Successfully acquired input: " + input + " [-]");
    return input;
}

std::string Program::Get::toolPath(const std::string& baseToolName) {
#if defined(_WIN32) || defined(__CYGWIN__)
    std::string toolName = baseToolName + ".exe";
#else
    std::string toolName = baseToolName;
#endif

    // checks AppImage runtime environment
    const char* appdir = std::getenv("APPDIR");
    if (appdir != nullptr) {
        std::filesystem::path bundledPath = std::filesystem::path(appdir) / "usr" / "bin" / toolName;
        if (std::filesystem::exists(bundledPath)) {
            return "\"" + bundledPath.string() + "\"";
        }
    }

    // maybe in the future, bundling runtime stuff for windows too
    std::filesystem::path localBundled = std::filesystem::path("bin") / toolName;
    if (std::filesystem::exists(localBundled)) {
        return "\"" + std::filesystem::absolute(localBundled).string() + "\"";
    }

    return toolName;
}