// fconvert v2.1.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#include "path_handler.h"
#include <iostream>
#include <algorithm>

bool PathHandler::debug_mode = false; // default debug mode is inactive, may change later

void PathHandler::log(const std::string& msg) {
    if (PathHandler::debug_mode) std::cout << std::endl << "[DEBUG] ------ " << msg << std::endl;
}

fs::path PathHandler::resolve_input(const std::string& filename) {
    // self explonatory, read the PathHandler::log calls if needed
    PathHandler::log("[-] Searching for the file: " + filename + " [-]");
    fs::path p(filename);
    PathHandler::log("[-] Checking: Is " + filename + " an absolute directory? [-]");
    if (p.is_absolute() && fs::exists(p)) {
        PathHandler::log("[-] Result: " + filename + " is an absolute directory. [-]");
        PathHandler::log("[-] Proceeding... [-]");
        return p;
    } else PathHandler::log("[-] Result: Input is NOT an absolute directory. [-]");

    fs::path local = fs::current_path() / filename;
    PathHandler::log("[-] Checking: Is " + filename + " in the current working directory? [-]");
    if (fs::exists(local)) {
        PathHandler::log("[-] Result: " + filename + " is in the current working directory. [-]");
        PathHandler::log("[-] Proceeding... [-]");
        return local;
    } else PathHandler::log("[-] Result: " + filename + " is NOT in the current local directory. [-]");

    fs::path sandbox = fs::current_path() / "input_file" / filename;
    PathHandler::log("[-] Checking: Is " + filename + " in the sandbox directory? [-]");
    if (fs::exists(sandbox)) {
        PathHandler::log("[-] Result: " + filename + " is in the sandbox directory. [-]");
        PathHandler::log("[-] Proceeding... [-]");
        return sandbox;
    } else PathHandler::log("[-] Result: " + filename + " is NOT in the sandbox directory. [-]");

    PathHandler::log("[!] Warning: Checks failed to find the file. [!]");
    std::cerr << "[!] Error: Could not find '" << filename << "' in the absolute, working, or sandbox directory. [!]" << std::endl;
    PathHandler::log("[!] Warning: Applying default, if any... [!]");
    PathHandler::log("[~] Status: This may fail. [~]");
    return fs::path(); // trusting the user
}

fs::path PathHandler::get_output_path(const fs::path& inputPath, const std::string& extension) {

    PathHandler::log("[~] Status: Defining the output directory. [~]");
    fs::path absoluteInput = fs::absolute(inputPath);
    fs::path inputDir = absoluteInput.parent_path();
    fs::path outDir;

    if (inputDir.filename() == "input_file") { // for sandbox conversion
        outDir = inputDir.parent_path() / "output_file";
    } else {
        outDir = inputDir; // the output will be provided in the same directory as the input
    }
    PathHandler::log("[-] Result: Output directory set to " + outDir.string() + "[-]");

    if (!outDir.empty() && !fs::exists(outDir)) {
        PathHandler::log("[!] Warning: " + outDir.string() + " does NOT exist. [!]");
        PathHandler::log("[~] Status: Creating the Directory! [~]");
        try {
            fs::create_directories(outDir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "[!] Permission denied: Could not create " << outDir.string() << " [!]\n"
                      << "[!] Warning: Check your folder permissions or run with appropriate privileges. [!]" << std::endl;
            PathHandler::log("[!] Error: Could not create directory " + outDir.string() + "[!]");
            return fs::path();
        }
        if(fs::exists(outDir)) PathHandler::log("[-] Result: Sucessfully created " + outDir.string() + " [-]");
    }

    std::string formattedExt;
    if (extension.empty()) {
        PathHandler::log("[!] Critical Error: Extension is empty. [!]");
        PathHandler::log("[!] Warning: Programm may be corrupt. Try restarting or reinstalling if this issue presists. [!]");
        throw std::runtime_error("[!] Critical Error: Empty extension provided to PathHandler. Internal error. ");
    }
    if (!extension.empty() && extension[0] == '.') {
        formattedExt = extension;
        PathHandler::log("[~] Detected: Extension already formatted. [~]");
    } else {
        formattedExt = "." + extension; // impossible case, repporting as a error catcher
        PathHandler::log("[~] Status: Prepending dot to extension: " + formattedExt);
    }
    fs::path out = outDir / absoluteInput.stem(); // full path + file name
    out.replace_extension(formattedExt); // appending extension
    PathHandler::log("[-] Result: Sucessfully defined output file directory and extension as: " + out.string());
    return out;
}

fs::path PathHandler::handle_conflicts(fs::path target, bool silent) {

    PathHandler::log("[~] Status: Handling naming conflicts. [~]");
    if (!fs::exists(target)) {
        PathHandler::log("[-] Result: PATH is available, no conflicts found. Skipping. [-]");
        return target;
    }
    
    if (!silent) {
        PathHandler::log("[!] Warning: File '" + target.string() + "' already exists, defaulting to conflict handler. [!]");
        std::cout << " [!] Conflict: " << target.filename().string() << " exists. (o)verwrite, (r)ename, (c)ancel: ";
        char c; 
        if (!(std::cin >> std::ws >> c)) {
            PathHandler::log("[!] Error: No input received. [!]");
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return "";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        c = std::tolower(static_cast<unsigned char>(c));
        PathHandler::log("[~] Status: Option '" + std::string(1, c) + "' chosen. Applying... [~]");
        if (c == 'o') return target;
        if (c == 'c') return "";
    }
    // program defaults to renaming for safety
    PathHandler::log("[~] Status: Initializing renaming logic. [~]");
    int i = 1; // index for renaminf file(1).mp3
    PathHandler::log("[~] Status: Acquiring full directory. [~]");
    fs::path baseDir = target.parent_path();
    PathHandler::log("[~] Status: Acquiring file name. [~]");
    std::string stem = target.stem().string();
    PathHandler::log("[~] Status: Acquiring extension. [~]");
    std::string ext = target.extension().string();
    
    PathHandler::log("[~] Status: Checking for available indexes. [~]");
    fs::path newPath = target;
    while (fs::exists(newPath)) { // testing indexes until one works without overwriting any existing file
        PathHandler::log("[~] Status: Testing index: " + std::to_string(i) + " [~]");
        newPath = baseDir / (stem + " (" + std::to_string(i++) + ")" + ext);
    }
    PathHandler::log("[-] Result: Index '" + std::to_string(i - 1) + "' succeeded. [-]");
    PathHandler::log("[~] Status: Saving as: " + newPath.string() + " [~]");
    return newPath;
}

std::string PathHandler::build_ffmpeg_cmd(const std::string& input, const std::string& output, const std::string& params) {
    PathHandler::log("[~] Status: Constructing the FFmpeg command for conversion. [~]");
    return "ffmpeg -hide_banner -i \"" + fs::path(input).make_preferred().string() + "\" " + 
           params + " \"" + fs::path(output).make_preferred().string() + "\" -y -loglevel error -stats";
}

std::string PathHandler::build_pandoc_cmd(const std::string& input, const std::string& output, const std::string& params) {
    PathHandler::log("[~] Status: Constructing the Pandoc command for conversion. [~]");
    return "pandoc \"" + fs::path(input).make_preferred().string() + "\" " + 
           params + " -o \"" + fs::path(output).make_preferred().string() + "\"";
}