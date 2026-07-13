// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_GET_HPP
#define PROGRAM_GET_HPP

#include <filesystem>
#include <string>

namespace Program {
    // currently only being use by ::Get, but may move to Program:: once it is used by multiple subclasses
    enum class Case { Normal, Lower, Upper }; // upper is unused atm, but it is there if I want to implement it
    enum class InputType { noWS, WS };

    namespace Get {
        std::filesystem::path logDirectory();
        std::string currentTimestamp();
        std::string input(const std::string& prompt, Case lower = Program::Case::Normal, InputType useWS = Program::InputType::WS);
    }
}

#endif