#ifndef PATH_HANDLER_H
#define PATH_HANDLER_H

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class PathHandler {
public:
    static bool debug_mode;
    static void log(const std::string& msg);
    static fs::path resolve_input(const std::string& filename);
    static fs::path get_output_path(const fs::path& inputPath, const std::string& extension);
    static fs::path handle_conflicts(fs::path target, bool silent = false);
    static std::string get_clean_cmd(const std::string& input, const std::string& output, const std::string& params);
};

#endif