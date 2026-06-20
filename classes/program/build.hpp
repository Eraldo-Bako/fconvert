// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_BUILD_HPP
#define PROGRAM_BUILD_HPP

#include <string>

namespace Program::Build {
        std::string command(const std::string& type, const std::string& input, const std::string& output, const std::string& params);
}


#endif