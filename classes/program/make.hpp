// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_MAKE_HPP
#define PROGRAM_MAKE_HPP

#include <string>
#include <filesystem>

namespace Program {
    namespace Make {
        std::filesystem::path logFile(const std::string& time);
        std::filesystem::path tempFile(const std::filesystem::path& original_inputPath);
    }
}

#endif