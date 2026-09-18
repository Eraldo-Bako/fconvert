// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef VIDEO_CONVERTER_HPP
#define VIDEO_CONVERTER_HPP

#include <filesystem>
#include <string>

bool check_ffmpeg();
void video();
void video_convert_logic(const std::filesystem::path& in, const std::string& fmt, const char q, const bool silent);

#endif