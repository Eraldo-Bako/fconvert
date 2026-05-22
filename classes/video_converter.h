// fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef VIDEO_CONVERTER_H
#define VIDEO_CONVERTER_H

#include <filesystem>
#include <string>

void video();
void video_convert_logic(std::filesystem::path in, std::string fmt, char q, bool silent);

#endif