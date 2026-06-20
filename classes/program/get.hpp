// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_GET_HPP
#define PROGRAM_GET_HPP

#include <filesystem>
#include <string>

namespace Program::Get {
    std::filesystem::path logDirectory();
    std::string currentTimestamp();
    std::string input(const std::string& prompt, bool lower = false, bool useWS = true);
}

#endif