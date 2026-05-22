// fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef IMAGE_CONVERTER_H
#define IMAGE_CONVERTER_H

#include <filesystem>
#include <string>

void image();
void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent);

#endif