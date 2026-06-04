// fconvert v2.2.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef VIDEO_CONVERTER_HPP
#define VIDEO_CONVERTER_HPP

#include <filesystem>
#include <string>

void video();
void video_convert_logic(std::filesystem::path in, std::string fmt, char q, bool silent);

#endif