#ifndef AUDIO_CONVERTER_H
#define AUDIO_CONVERTER_H

#include <filesystem>
#include <string>

void audio();
void audio_convert_logic(std::filesystem::path in, std::string fmt, bool silent);

#endif