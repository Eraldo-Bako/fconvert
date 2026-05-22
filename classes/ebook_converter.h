// fconvert v2.0.3 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef EBOOK_CONVERTER_H
#define EBOOK_CONVERTER_H

#include <iostream>
#include <string>
#include <filesystem>
#include <set>

namespace fs = std::filesystem;

class EbookConverter {
public:
    static bool check_pandoc();
    static bool convert(const fs::path& input, const std::string& target_ext);

private:
    static std::string build_pandoc_cmd(const std::string& in, const std::string& out, const std::string& args);
};

void ebook();
void ebook_convert_logic(fs::path in, std::string fmt, bool silent);

#endif