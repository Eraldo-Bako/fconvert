#include "audio_converter.h"
#include "path_handler.h"
#include <iostream>
#include <algorithm>

void audio_convert_logic(fs::path in, std::string fmt, bool silent) {
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), ::tolower);
    fs::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    std::string params;
    if (fmt == "flac" || fmt == "wav") {
        params = "-c:a flac";
    } else if (fmt == "mp3") {
        params = "-c:a libmp3lame -q:a 0";
    } else if (fmt == "ogg") {
        params = "-c:a libvorbis -q:a 5";
    } else if (fmt == "m4a") {
        params = "-c:a aac -b:a 256k";
    } else if (fmt == "opus") {
        params = "-c:a libopus -b:a 128k";
    } else {
        params = "-c:a aac -b:a 192k";
    }

    std::string cmd = PathHandler::get_clean_cmd(in.string(), out.string(), params);
    std::cout << " [*] Converting Audio..." << std::endl;
    std::system(cmd.c_str());
}

void audio() {
    std::string name, fmt;
    std::cout << "Audio filename: "; std::getline(std::cin >> std::ws, name);
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) return;
    std::cout << "Format: "; std::cin >> fmt;
    audio_convert_logic(in, fmt, false);
}