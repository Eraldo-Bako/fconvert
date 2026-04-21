#include "video_converter.h"
#include "path_handler.h"
#include <iostream>
#include <algorithm>

void video_convert_logic(fs::path in, std::string fmt, char q, bool silent) {
    std::transform(fmt.begin(), fmt.end(), fmt.begin(), ::tolower);
    fs::path target = PathHandler::get_output_path(in, "." + fmt);
    fs::path out = PathHandler::handle_conflicts(target, silent);
    if (out.empty()) return;

    std::string params;
    if (fmt == "webm") {
        if (q == 'b')      params = "-c:v libvpx-vp9 -crf 20 -b:v 0 -deadline best -c:a libopus";
        else if (q == 'q') params = "-c:v libvpx-vp9 -crf 35 -b:v 0 -deadline realtime -c:a libopus";
        else               params = "-c:v libvpx-vp9 -crf 30 -b:v 0 -deadline good -c:a libopus";
    } else {
        if (q == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a aac -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a aac -b:a 160k";
    }

    std::string cmd = PathHandler::get_clean_cmd(in.string(), out.string(), params);
    PathHandler::log("CMD: " + cmd);
    std::cout << " [*] Converting Video..." << std::endl;
    std::system(cmd.c_str());
}

void video() {
    std::string name, fmt, qual;
    std::cout << "Video filename: "; std::getline(std::cin >> std::ws, name);
    fs::path in = PathHandler::resolve_input(name);
    if (in.empty()) return;
    std::cout << "Format (mp4/webm): "; std::cin >> fmt;
    std::cout << "Quality ([B]est, [G]ood, [Q]uick): "; std::cin >> qual;
    video_convert_logic(in, fmt, std::tolower(qual[0]), false);
}