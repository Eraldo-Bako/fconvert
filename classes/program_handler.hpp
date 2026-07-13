// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_HANDLER_HPP
#define PROGRAM_HANDLER_HPP

#include <filesystem>
#include <string>
#include <string_view>

namespace Program {

    enum class PrintType { Normal, Error };
    enum class LogDest { None, Console, File, All };

    inline std::filesystem::path activeLogPath = "";
    inline bool debug_mode = false;

    [[noreturn]] void end(const std::string& msg);
    void log(const std::string_view msg, LogDest dest = LogDest::All);
    void print(const std::string_view msg, PrintType type = PrintType::Normal);
    void clearCache();

}

#include "program/build.hpp"
#include "program/check.hpp"
#include "program/color.hpp"
#include "program/get.hpp"
#include "program/make.hpp"

#endif