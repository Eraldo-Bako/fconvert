// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "../program_handler.hpp"
#include "build.hpp"
#include "get.hpp"

#include <filesystem>
#include <iostream>


std::string Program::Build::command(const std::string& type, const std::string& input, const std::string& output, const std::string& params) {
    std::string cleanInput = std::filesystem::path(input).make_preferred().string();
    std::string cleanOutput = std::filesystem::path(output).make_preferred().string();
    
    if (type == "ffmpeg") {
        Program::log("[~] Status: Constructing the FFmpeg command for conversion. [~]");
        std::string ffmpegPATH = Program::Get::toolPath("ffmpeg");

        return ffmpegPATH + " -hide_banner -i \"" + 
               cleanInput + "\" " + 
               params + " \"" + 
               cleanOutput + "\" -y -loglevel error -stats";
    } else if (type == "pandoc") {
        Program::log("[~] Status: Constructing the Pandoc command for conversion. [~]");
        std::string pandocPATH = Program::Get::toolPath("pandoc");

        return pandocPATH + " \"" + 
               cleanInput + "\" " + 
               params + " -o \"" + 
               cleanOutput + "\"";
    } else if (type == "magick") {
        Program::log("[~] Status: Constructing the ImageMagick command for conversion. [~]");
        std::string magickPATH = Program::Get::toolPath("magick");

        if (params == "fast") 
            return magickPATH + " \"" + 
                   cleanInput + "[0]\" \"" + 
                   cleanOutput + "\"";
        else if (params == "layered") 
            return magickPATH + " -density 300 -background none \"" + 
                   cleanInput + "\" \"" + 
                   cleanOutput + "\"";
        else {
            std::string extraParams = params.empty() ? "" : params + " ";
            return magickPATH + " " + 
                   extraParams + " \"" + 
                   cleanInput + "\" \"" + 
                   cleanOutput + "\"";
        }
    } else if (type == "vtracer") {
        Program::log("[~] Status: Constructing the VTracer command for conversion. [~]");
        std::string vtracerPATH = Program::Get::toolPath("vtracer");

        return vtracerPATH + 
               " --input \"" + cleanInput + 
               "\" --output \"" + cleanOutput + "\" " + 
               params;

    }
    std::cerr << "[!] Critical Error: Program couldn't define a conversion method. [!]";
    return "";
}