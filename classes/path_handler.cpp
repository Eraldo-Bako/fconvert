#include "path_handler.h"
#include <iostream>
#include <algorithm>

bool PathHandler::debug_mode = false;

void PathHandler::log(const std::string& msg) {
    if (debug_mode) std::cout << " [DEBUG] " << msg << std::endl;
}

fs::path PathHandler::resolve_input(const std::string& filename) {
    fs::path p(filename);
    
    // 1. If absolute path is given and exists
    if (p.is_absolute() && fs::exists(p)) return p;

    // 2. Check Sandbox: CWD/input_file/filename
    fs::path sandbox = fs::current_path() / "input_file" / filename;
    if (fs::exists(sandbox)) return sandbox;

    // 3. Check CWD: CWD/filename
    fs::path local = fs::current_path() / filename;
    if (fs::exists(local)) return local;

    return "";
}

fs::path PathHandler::get_output_path(const fs::path& inputPath, const std::string& extension) {
    // Determine the absolute parent of the input
    fs::path absoluteInput = fs::absolute(inputPath);
    fs::path inputDir = absoluteInput.parent_path();
    fs::path outDir;

    // Check if the input is inside an "input_file" directory
    if (inputDir.filename() == "input_file") {
        // Output to sibling "output_file"
        outDir = inputDir.parent_path() / "output_file";
    } else {
        // Output to the same directory as input
        outDir = inputDir;
    }

    // Ensure directory exists
    if (!outDir.empty() && !fs::exists(outDir)) {
        fs::create_directories(outDir);
    }

    fs::path out = outDir / absoluteInput.stem();
    out.replace_extension(extension);
    return out;
}

fs::path PathHandler::handle_conflicts(fs::path target, bool silent) {
    if (!fs::exists(target)) return target;
    
    if (!silent) {
        std::cout << " [!] Conflict: " << target.filename().string() << " exists. (o)verwrite, (r)ename, (c)ancel: ";
        char c; 
        if (!(std::cin >> c)) return "";
        c = std::tolower(c);
        if (c == 'o') return target;
        if (c == 'c') return "";
    }

    // Auto-rename logic
    int i = 1;
    fs::path baseDir = target.parent_path();
    std::string stem = target.stem().string();
    std::string ext = target.extension().string();
    
    fs::path newPath = target;
    while (fs::exists(newPath)) {
        newPath = baseDir / (stem + " (" + std::to_string(i++) + ")" + ext);
    }
    return newPath;
}

std::string PathHandler::get_clean_cmd(const std::string& input, const std::string& output, const std::string& params) {
    return "ffmpeg -hide_banner -i \"" + fs::path(input).make_preferred().string() + "\" " + 
           params + " \"" + fs::path(output).make_preferred().string() + "\" -y -loglevel error -stats";
}