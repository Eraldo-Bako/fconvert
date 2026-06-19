// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PATH_HANDLER_HPP
#define PATH_HANDLER_HPP

#include "program_handler.hpp"

#include <filesystem>
#include <string>

class PathHandler {
public:
    static std::filesystem::path resolve_input(const std::string& filename);
    static std::filesystem::path get_output_path(const std::filesystem::path& inputPath, const std::string& extension);
    static std::filesystem::path handle_conflicts(std::filesystem::path target, bool silent = false);
};

#endif