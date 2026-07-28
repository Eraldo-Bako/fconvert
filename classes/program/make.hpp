// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_MAKE_HPP
#define PROGRAM_MAKE_HPP

#include <string>
#include <filesystem>

namespace Program::Make {
    std::filesystem::path logFile(const std::string& time);
}

#endif