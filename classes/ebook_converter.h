#ifndef EBOOK_CONVERTER_H
#define EBOOK_CONVERTER_H

#include <filesystem>
#include <string>

void ebook();
void ebook_convert_logic(std::filesystem::path in, std::string fmt, bool silent);

#endif