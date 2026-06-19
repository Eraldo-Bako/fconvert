// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_HANDLER_HPP
#define PROGRAM_HANDLER_HPP

#include <filesystem>
#include <string>

namespace Program {
    inline bool debug_mode = false;
    inline std::filesystem::path activeLogPath = "";

    [[noreturn]] void end(const std::string& msg);
    void log(const std::string& msg, bool print = true, bool write = true);
    void print(const std::string& msg, bool error = false);
    
}

#include "program/build.hpp"
#include "program/check.hpp"
#include "program/get.hpp"
#include "program/make.hpp"

#endif

// Program::debug_mode
// Program::activeLogPath
// Program::log("Hello")


// Program::Get::currentTimestamp()
// Program::Get::input("What is your input? ", lower = true)

// Program::Make::tempFile(path/to/original/file.ext)
// Program::Make::logFile("formatted time")

// Program::Build::cmd("ffmpeg", "file.png", "file.jpg", "-flags --and-extra --stuff")