#include "audio_converter.h"
#include "path_handler.h"
#include <iostream>
#include <algorithm>
#include <set>

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
        params = "-c:a aac -b:a 256k -movflags +faststart";
    } else if (fmt == "aac") {
        params = "-fflags +genpts -vn -sn -dn -c:a aac -b:a 256k -af \"aresample=async=1\"";
    } else if (fmt == "opus") {
        params = "-c:a libopus -b:a 128k";
    } else {
        params = "-c:a aac -b:a 192k -ar 44100";
    }

    std::string cmd = PathHandler::get_clean_cmd(in.string(), out.string(), params);
    std::cout << " [*] Converting Audio..." << std::endl;
    std::system(cmd.c_str());
}

void audio() {
    std::string name, fmt;
    std::cout << "Audio filename or path: "; std::getline(std::cin >> std::ws, name);
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) return;

    std::cout << "Format: "; if (!(std::cin >> fmt)) return;
    for (auto &c : fmt) c = std::tolower(c);

    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    static const std::set<std::string> valid_audio = {"mp3", "wav", "flac", "aac", "ogg", "m4a", "wma", "opus"};
    if (valid_audio.find(fmt) == valid_audio.end()) {
        std::cout << "\n[!] Format '" << fmt << "' is not supported or doesn't exist. [!]\n"
                  << "Supported audio formats include:\n"
                  << "* Uncompressed: WAV\n"
                  << "* Lossless Compressed: FLAC\n"
                  << "* Lossy Compressed: MP3, AAC, OGG, M4A, WMA, OPUS\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }

    audio_convert_logic(in, fmt, false);

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}