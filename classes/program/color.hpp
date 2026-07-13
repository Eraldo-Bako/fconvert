// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_COLOR_HPP
#define PROGRAM_COLOR_HPP

#include <string_view>

namespace Program::Color {

    inline constexpr std::string_view RESET = "\033[0m";
    inline constexpr std::string_view CYAN = "\033[36m";
    inline constexpr std::string_view GREEN = "\033[32m";
    inline constexpr std::string_view YELLOW = "\033[33m";
    inline constexpr std::string_view RED = "\033[1;31m";

}

#endif