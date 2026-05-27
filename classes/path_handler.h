// fconvert v2.1.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef PATH_HANDLER_H
#define PATH_HANDLER_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class PathHandler {
public:
    static bool debug_mode;
    static void log(const std::string& msg);
    static fs::path resolve_input(const std::string& filename);
    static fs::path get_output_path(const fs::path& inputPath, const std::string& extension);
    static fs::path handle_conflicts(fs::path target, bool silent = false);
    static std::string build_ffmpeg_cmd(const std::string& input, const std::string& output, const std::string& params);
    static std::string build_pandoc_cmd(const std::string& input, const std::string& output, const std::string& params);
};

#endif