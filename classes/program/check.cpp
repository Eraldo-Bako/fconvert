// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "check.hpp"

#include <cstdlib>

bool Program::Check::command_exists(const std::string& win_cmd, const std::string& nix_cmd) {
#ifdef _WIN32
    return std::system((win_cmd + " >nul 2>nul").c_str()) == 0;
#else
    return std::system((nix_cmd + " >/dev/null 2>&1").c_str()) == 0;
#endif
}

bool Program::Check::ffmpeg() {
    return Program::Check::command_exists("where ffmpeg", "command -v ffmpeg");
}

bool Program::Check::imghost() {
    return Program::Check::command_exists("where magick", "command -v magick") && 
           Program::Check::command_exists("where gswin64c", "command -v gs");
}

bool Program::Check::libraw() {
#ifdef _WIN32
    return true; 
#else
    return Program::Check::command_exists("", "pkg-config --exists libraw");
#endif
}

bool Program::Check::opencv() {
#if defined(_WIN32) || defined(__APPLE__)
    return true; 
#else
    return Program::Check::command_exists("", "pkg-config --exists opencv4");
#endif
}

bool Program::Check::pandoc() {
    return Program::Check::command_exists("where pandoc", "command -v pandoc");
}

bool Program::Check::pdfEngine() {
    if (Program::Check::command_exists("where typst", "command -v typst")) return true;
    if (Program::Check::command_exists("where weasyprint", "command -v weasyprint")) return true;
    if (Program::Check::command_exists("where xelatex", "command -v xelatex")) return true;
    return false;
}