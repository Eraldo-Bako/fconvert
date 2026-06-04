// fconvert v2.2.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef EBOOK_CONVERTER_HPP
#define EBOOK_CONVERTER_HPP

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class EbookConverter {
public:
    static bool check_pandoc();
    static bool convert(const fs::path& input, const std::string& fmt);
};

void ebook();
void ebook_convert_logic(fs::path in, std::string fmt, bool silent);

#endif