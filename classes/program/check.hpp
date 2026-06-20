// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_CHECK_HPP
#define PROGRAM_CHECK_HPP

#include <string>

namespace Program::Check {
        bool command_exists(const std::string& win_cmd, const std::string& nix_cmd);
        bool ffmpeg();
        bool imghost();
        bool libraw();
        bool opencv();
        bool pandoc();
        bool pdfEngine();
}

#endif