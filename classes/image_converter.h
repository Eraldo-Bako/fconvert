#ifndef IMAGE_CONVERTER_H
#define IMAGE_CONVERTER_H

#include <filesystem>
#include <string>

void image();
void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent);

#endif