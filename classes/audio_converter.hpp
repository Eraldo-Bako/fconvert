// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef AUDIO_CONVERTER_HPP
#define AUDIO_CONVERTER_HPP

#include <filesystem>
#include <string>

void audio();
void audio_convert_logic(std::filesystem::path in, std::string fmt, bool silent);

#endif