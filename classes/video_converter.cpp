// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "video_converter.hpp"
#include "path_handler.hpp"
#include "program_handler.hpp"
#include "secure_conversion_session.hpp"

#include <iostream>
#include <algorithm>
#include <set>
#include <fmt/core.h>

#ifndef _WIN32
    #include <sys/wait.h>
#endif

void video_convert_logic(const std::filesystem::path& in, const std::string& fmt, const char quality, const bool silent) {
    if (!Program::Check::ffmpeg()) {
        Program::print(_("[!] Error: FFmpeg not found. [!]\n"), Program::PrintType::Error);
        return;
    }

    std::filesystem::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    SecureConversionSession session(in, fmt);

    std::string params;
    if (fmt == "mp4" || fmt == "m4v" || fmt == "f4v" || fmt == "mov") {
        if (quality == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a aac -b:a 192k";
        else if (quality == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a aac -b:a 160k";
    } else if (fmt == "mkv") {
        if (quality == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a libopus -b:a 192k";
        else if (quality == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a libopus -b:a 96k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a libopus -b:a 128k";
    } else if (fmt == "webm") {
        if (quality == 'b')      params = "-c:v libvpx-vp9 -crf 20 -b:v 0 -deadline best -c:a libopus -b:a 192k";
        else if (quality == 'q') params = "-c:v libvpx-vp9 -crf 35 -b:v 0 -deadline realtime -c:a libopus -b:a 96k";
        else               params = "-c:v libvpx-vp9 -crf 30 -b:v 0 -deadline good -c:a libopus -b:a 128k";
    } else if (fmt == "avi") {
        if (quality == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a mp3 -b:a 192k";
        else if (quality == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a mp3 -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a mp3 -b:a 160k";
    } else if (fmt == "wmv") {
        if (quality == 'b')      params = "-c:v wmv2 -b:v 4M -c:a wmav2 -b:a 192k";
        else if (quality == 'q') params = "-c:v wmv2 -b:v 1M -c:a wmav2 -b:a 96k";
        else               params = "-c:v wmv2 -b:v 2M -c:a wmav2 -b:a 128k";
    } else if (fmt == "flv") {
        if (quality == 'b')      params = "-c:v libx264 -crf 19 -preset slow -c:a aac -b:a 160k";
        else if (quality == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 96k";
        else               params = "-c:v libx264 -crf 23 -preset medium -c:a aac -b:a 128k";
    } else if (fmt == "mpg" || fmt == "mpeg") {
        if (quality == 'b')      params = "-c:v mpeg2video -b:v 8M -maxrate 9M -bufsize 2M -c:a mp2 -b:a 224k";
        else if (quality == 'q') params = "-c:v mpeg2video -b:v 2M -maxrate 3M -bufsize 1M -c:a mp2 -b:a 128k";
        else               params = "-c:v mpeg2video -b:v 5M -maxrate 6M -bufsize 2M -c:a mp2 -b:a 160k";
    } else if (fmt == "3gp" || fmt == "3g2") {
        if (quality == 'b')      params = "-c:v h263 -b:v 500k -r 15 -s cif -c:a libopencore_amrnb -b:a 12.2k -ar 8000 -ac 1";
        else if (quality == 'q') params = "-c:v h263 -b:v 150k -r 10 -s qcif -c:a libopencore_amrnb -b:a 4.75k -ar 8000 -ac 1";
        else               params = "-c:v h263 -b:v 300k -r 15 -s qcif -c:a libopencore_amrnb -b:a 7.4k -ar 8000 -ac 1";
    } else if (fmt == "avchd" || fmt == "mts" || fmt == "m2ts") {
        if (quality == 'b')      params = "-c:v libx264 -crf 18 -preset slow -c:a ac3 -b:a 192k";
        else if (quality == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a ac3 -b:a 128k";
        else               params = "-c:v libx264 -crf 23 -preset medium -c:a ac3 -b:a 160k";
    } else if (fmt == "ogv" || fmt == "ogg") {
        if (quality == 'b')      params = "-c:v libtheora -q:v 7 -c:a libvorbis -q:a 6";
        else if (quality == 'q') params = "-c:v libtheora -q:v 3 -c:a libvorbis -q:a 3";
        else               params = "-c:v libtheora -q:v 5 -c:a libvorbis -q:a 5";
    } else if (fmt == "prores") {
        if (quality == 'b')      params = "-c:v prores_ks -profile:v 3 -vendor ap10 -pix_fmt yuv422p10le -c:a pcm_s16le";
        else if (quality == 'q') params = "-c:v prores_ks -profile:v 0 -vendor ap10 -pix_fmt yuv422p10le -c:a pcm_s16le";
        else               params = "-c:v prores_ks -profile:v 2 -vendor ap10 -pix_fmt yuv422p10le -c:a pcm_s16le";
    } else if (fmt == "dnxhd" || fmt == "dnxhr") {
        if (quality == 'b')      params = "-c:v dnxhd -profile:v dnxhr_hquality -pix_fmt yuv422p -c:a pcm_s16le";
        else if (quality == 'q') params = "-c:v dnxhd -profile:v dnxhr_lb -pix_fmt yuv422p -c:a pcm_s16le";
        else               params = "-c:v dnxhd -profile:v dnxhr_squality -pix_fmt yuv422p -c:a pcm_s16le";
    } else { //intentional repetition of the first if block, may change in the future ~ kinda unreachable
        if (quality == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a aac -b:a 192k";
        else if (quality == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a aac -b:a 160k";
    }

    std::string cmd = Program::Build::command(Program::Build::cmdType::FFmpeg, session.safe_input(), session.safe_output(), params);
    Program::log(fmt::format(_("[-] Executing Video Conversion: {0} [-]"), cmd));
    Program::print(_("[~] Status: Converting Video... [~]\n"));

    bool success = false; // error catching -_-
    if (int execute = std::system(cmd.c_str()); execute == -1) { // the system shell itself couldn't be started, critical
        std::cerr << _("[!] Critical Error: Failed to initiate the command shell. [!]\n");
    } else { // did the command even finish normally?
        #ifdef _WIN32
            if (execute == 0) {
                std::cout << _("[~] Status: Conversion completed successfully! [~]") << std::endl;
                success = true;
            } else {
                std::cerr << _("[!] Error: FFmpeg failed with exit code: ") << execute << std::endl;
            }
        #else
            if (WIFEXITED(execute)) {
                if (int exitCode = WEXITSTATUS(execute); exitCode == 0) { //successful conversion
                    Program::print(_("[~] Status: Conversion completed successfully! [~]\n"));
                    success = true;
                } else { // either the constructed cmd is wrong or FFmpeg is acting up
                    Program::print(fmt::format(_("[!] Error: FFmpeg failed with exit code: {0} [!]\n"), std::to_string(exitCode)), Program::PrintType::Error);
                }
            } else { // so FFmpeg was terminated either by the user or the system itself(might have crashed)
                Program::print(_("[!] Error: FFmpeg was terminated abnormally. [!]\n"), Program::PrintType::Error);
                Program::print(_("[~] If you believe this is a bug, please report it. [~]\n"));
                Program::print(_("[~] Run fconvert -h or --help for more instructions. [~]\n"));
            }
        #endif
    }

    if (success) {
        if (session.commit(out)) {
            Program::print(_("[~] Status: Conversion completed successfully! [~]\n"));
        } else {
            Program::print(_("[!] Error: Failed to safely export output file from sandbox. [!]\n"), Program::PrintType::Error);
        }
    } else {
        Program::print(_("[!] Error: FFmpeg execution failed or terminated prematurely. [!]\n"), Program::PrintType::Error);
    }
}

void video() {
    std::vector<Program::Get::ParsedInput> raw_inputs =
        Program::Get::multipleInput(Program::Get::input(_("Video filename(s) or path(s): ")));
    
    if (raw_inputs.empty()) {
        Program::log(_("[!] Error: No input paths provided. [!]"));
        return;
    }

    std::string fmt = Program::Get::input(_("Format: "), Program::Case::Lower);
    if (fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        Program::log(fmt::format(_("[~] Detected: {0} [~]\n[~] Quitting... [~]"), fmt));
        Program::print(_("[!] Successfully stopped the conversion! [!]"));
        return;
    }

    static const std::set<std::string> valid_video = {"mp4", "mov", "avi", "wmv", "flv", 
                                                      "f4v", "mkv", "webm", "3gp", "3g2", 
                                                      "m4v", "f4v", "mpg", "mpeg", "avchd", "mts",
                                                      "m2ts", "ogv", "ogg", "prores", "dnxhd", 
                                                      "dnxhr"};
    if (valid_video.find(fmt) == valid_video.end()) {
        Program::print(fmt::format(_("\n[!] Format '{0}' is not supported or doesn't exist. [!]\n"), fmt));
        std::cout << _("Supported video formats include:\n")
                  << "MP4 (H.264), MOV, AVI, WMV, FLV, F4V, MKV, WebM, 3GP & 3G2,\n"
                  << "M4V, MPEG-2, AVCHD, MTS, M2TS, OGV, OGG, PRORES, DNXHD, DNXHR\n"
                  << _("\n[-] If you believe this is a bug, make sure to report it. [-]\n");
        return;
    }

    std::string qual = 
        Program::Get::input(
            _("Select Quality ([Q]uick, [D]efault, [B]est): "), 
            Program::Case::Lower
        );
    if (qual == "quit" || qual == "exit" || qual == "cancel") {
        std::cout << _("[!] Successfully stopped the conversion! [!]");
        return;
    }

    if(qual.empty() || qual[0] != 'q' || qual[0] != 'd' || qual[0] != 'b') {
        Program::print(fmt::format(_("[!] Invalid quality option provided: {0} [!]\nExiting..."), qual));
        return;
    }

    for (const auto& raw_input : raw_inputs) {
        // should access the .path member of the ParsedInput struct
        std::filesystem::path in = PathHandler::resolve_input(raw_input.path);
        
        if (in.empty()) {
            std::cout << fmt::format(_("[!] Warning: Path could not be resolved for '{0}'. Skipping...\n"), raw_input.path);
            Program::log(fmt::format(_("[!] Path resolution failed for: {0}"), raw_input.path));
            continue;
        }

        // Actual conversion logic executed 1 by 1
        video_convert_logic(in, fmt, qual[0], false);
    }
}