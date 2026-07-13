// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "build.hpp"
#include "../program_handler.hpp"

#include <filesystem>
#include <iostream>


std::string Program::Build::command(const std::string& type, const std::string& input, const std::string& output, const std::string& params) {
    if (type == "ffmpeg") {
        Program::log("[~] Status: Constructing the FFmpeg command for conversion. [~]");
        return "ffmpeg -hide_banner -i \"" + std::filesystem::path(input).make_preferred().string() + "\" " + 
            params + " \"" + std::filesystem::path(output).make_preferred().string() + 
            "\" -y -loglevel error -stats";
    } else if (type == "pandoc") {
        Program::log("[~] Status: Constructing the Pandoc command for conversion. [~]");
        return "pandoc \"" + std::filesystem::path(input).make_preferred().string() + "\" " + 
            params + " -o \"" + std::filesystem::path(output).make_preferred().string() + "\"";
    }
    std::cerr << "[!] Critical Error: Program couldn't define a conversion method. [!]";
    return "";
}