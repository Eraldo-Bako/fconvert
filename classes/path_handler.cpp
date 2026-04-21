#include "path_handler.h"
#include <iostream>
#include <algorithm>

bool PathHandler::debug_mode = false;

void PathHandler::log(const std::string& msg) {
    if (debug_mode) std::cout << " [DEBUG] " << msg << std::endl;
}

fs::path PathHandler::resolve_input(const std::string& filename) {
    fs::path p(filename);
    if (fs::exists(p)) return p;
    fs::path sandbox = fs::path("input_file") / filename;
    if (fs::exists(sandbox)) return sandbox;
    return "";
}

fs::path PathHandler::get_output_path(const fs::path& inputPath, const std::string& extension) {
    fs::path outDir = inputPath.parent_path();
    // Sandbox check
    fs::path sandboxDir = fs::absolute("input_file");
    if (fs::exists(sandboxDir) && fs::equivalent(inputPath.parent_path(), sandboxDir)) {
        outDir = "output_file";
    }
    
    if (!fs::exists(outDir)) fs::create_directories(outDir);
    
    fs::path out = outDir / inputPath.stem();
    out.replace_extension(extension);
    return out;
}

fs::path PathHandler::handle_conflicts(fs::path target, bool silent) {
    if (!fs::exists(target)) return target;
    if (!silent) {
        std::cout << " [!] Conflict: " << target.filename().string() << " exists. (o)verwrite, (r)ename, (c)ancel: ";
        char c; std::cin >> c;
        if (std::tolower(c) == 'o') return target;
        if (std::tolower(c) == 'c') return "";
    }
    // Auto-rename logic
    int i = 1;
    fs::path base = target.parent_path() / target.stem();
    fs::path ext = target.extension();
    fs::path newPath = target;
    while (fs::exists(newPath)) {
        newPath = base.string() + " (" + std::to_string(i++) + ")" + ext.string();
    }
    return newPath;
}

std::string PathHandler::get_clean_cmd(const std::string& input, const std::string& output, const std::string& params) {
    // make_preferred() handles \ for Windows and / for Linux automatically
    return "ffmpeg -hide_banner -i \"" + fs::path(input).make_preferred().string() + "\" " + 
           params + " \"" + fs::path(output).make_preferred().string() + "\" -y -loglevel error -stats";
}