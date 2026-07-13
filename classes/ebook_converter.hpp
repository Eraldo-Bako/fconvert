// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef EBOOK_CONVERTER_HPP
#define EBOOK_CONVERTER_HPP

#include <string>
#include <filesystem>

class EbookConverter {
public:
    static bool convert(const std::filesystem::path& input, const std::string& fmt);
};

void ebook();
void ebook_convert_logic(std::filesystem::path in, std::string fmt, bool silent);

#endif