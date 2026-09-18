// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef AUDIO_CONVERTER_HPP
#define AUDIO_CONVERTER_HPP

#include "program_handler.hpp"

#include <filesystem>
#include <string>

void audio();
void audio_convert_logic(const std::filesystem::path& in, const std::string& fmt, const bool silent);

#endif