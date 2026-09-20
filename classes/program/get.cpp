// fconvert v2.4.1-rc3 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "get.hpp"
#include "../program_handler.hpp"

#include <cctype>
#include <ctime>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h>

#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#include <climits>
#endif

std::string Program::Get::localeDirectory() {
    const char* appdir = std::getenv("APPDIR");
    if (appdir) return std::string(appdir) + LOCALEDIR;
    std::filesystem::path exec_dir;

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
            exec_dir = exec_path.parent_path();

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

    while (!input.empty() && (input.back() == '\r' || input.back() == '\n')) {
        input.pop_back();
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

std::vector<Program::Get::ParsedInput> Program::Get::multipleInput(const std::string& fullInput) {
    std::vector<Program::Get::ParsedInput> tokens;
    // blind/basic estimation pre allocation
    size_t delimiter_count = std::count(fullInput.begin(), fullInput.end(), ',') +
                             std::count(fullInput.begin(), fullInput.end(), '|');
    tokens.reserve(delimiter_count + 1);

    // Helper lambda to process and push a verified token
    auto push_token = [&](const std::string& s_l_path_str) {
        std::string l_ext = std::filesystem::path(s_l_path_str).extension().string();
        
        // Convert to lowercase to ensure consistency (.JPG -> .jpg)
        std::transform(l_ext.begin(), l_ext.end(), l_ext.begin(), [](unsigned char c){ return std::tolower(c); });

        // If no extension was found, read the magic bytes
        if (l_ext.empty()) {
            l_ext = Program::Get::extensionFromHeader(s_l_path_str);
        }

        tokens.push_back({s_l_path_str, l_ext});
    };

    // Helper lambda to trim whitespace and strip surrounding quotes
    auto clean_segment = [](std::string_view l_segment) -> std::string_view {
        size_t l_start = l_segment.find_first_not_of(" \t\n\r");
        if (l_start == std::string::npos) return {};
        size_t l_end = l_segment.find_last_not_of(" \t\n\r");
        l_segment = l_segment.substr(l_start, l_end - l_start + 1);

        if (l_segment.size() >= 2 && (
                (l_segment.front() == '"' && l_segment.back() == '"') || 
                (l_segment.front() == '\'' && l_segment.back() == '\'')
            )) 
        {
            //l_segment = l_segment.substr(1, l_segment.size() - 2);
            l_segment.remove_prefix(1);
            l_segment.remove_suffix(1);
        }
        return l_segment;
    };

    // Helper lambda that clans and pushes the token
    auto process_segment = [&](const std::string_view s_l_raw) {
        std::string_view l_path_str = clean_segment(s_l_raw);
        if (!l_path_str.empty() && std::filesystem::exists(l_path_str)) {
            push_token(std::string(l_path_str));
        }
    };

    // Expected '|' delimiter detected
    if (fullInput.find('|') != std::string::npos) {
        size_t segment_start = 0;
        size_t segment_end = 0;

        while ((segment_end = fullInput.find('|', segment_start)) != std::string::npos) {
            process_segment(std::string_view(fullInput).substr(
                                segment_start,
                                segment_end - segment_start
                            ));
            segment_start = segment_end + 1;
        }
        process_segment(std::string_view(fullInput).substr(segment_start));
        return tokens;
    }

    // Potentially expected: ('input', 'input') or ("input", "input")
    bool has_single_delim = (fullInput.find("', '") != std::string::npos) || 
                            (fullInput.find("','")   != std::string::npos);
    bool has_double_delim = (fullInput.find("\", \"") != std::string::npos) ||
                            (fullInput.find("\",\"") != std::string::npos);

    if (has_single_delim || has_double_delim) {
        size_t segment_start = 0;
        size_t segment_end = 0;

        while ((segment_end = fullInput.find(',', segment_start)) != std::string::npos) {
            process_segment(std::string_view(fullInput).substr(
                                segment_start,
                                segment_end - segment_start
                            ));
            segment_start = segment_end + 1;
        }
        process_segment(std::string_view(fullInput).substr(segment_start));
        return tokens;
    }

    // Fallback, trying to decipher the input
    std::string current_token;
    bool inside_quotes = false;
    char quote_char = '\0';

    for (size_t i = 0; i < fullInput.length(); ++i) {
        char c = fullInput[i];

        if ((c == '"' || c == '\'') && !inside_quotes && current_token.empty()) {
            inside_quotes = true;
            quote_char = c;
            continue; 
        }

        if (inside_quotes && c == quote_char) {
            inside_quotes = false;
            quote_char = '\0';
            continue; 
        }

        if (std::isspace(static_cast<unsigned char>(c)) && !inside_quotes) {
            if (!current_token.empty()) {
                if (std::filesystem::exists(current_token)) {
                    push_token(current_token); // Use the helper
                    current_token.clear();
                } else {
                    current_token += c;
                }
            }
        } else {
            current_token += c;
        }
    }

    if (!current_token.empty()) {
        if (std::filesystem::exists(current_token)) {
            push_token(current_token);
        } else if (tokens.empty()) {
            // Ultimate fallback if nothing exists and it's just a raw string
            tokens.push_back({fullInput, ""});
        }
    }

    return tokens;
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