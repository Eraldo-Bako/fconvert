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

#ifndef _WIN32
    #include <sys/wait.h>
#endif

void video_convert_logic(std::filesystem::path in, std::string fmt, char q, bool silent) {
    if (!Program::Check::ffmpeg()) {
        Program::print("[!] Error: FFmpeg not found. [!]\n", Program::PrintType::Error);
        return;
    }

    std::filesystem::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    SecureConversionSession session(in, fmt);

    std::string params;
    if (fmt == "mp4" || fmt == "m4v" || fmt == "f4v" || fmt == "mov") {
        if (q == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a aac -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a aac -b:a 160k";
    } else if (fmt == "mkv") {
        if (q == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a libopus -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a libopus -b:a 96k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a libopus -b:a 128k";
    } else if (fmt == "webm") {
        if (q == 'b')      params = "-c:v libvpx-vp9 -crf 20 -b:v 0 -deadline best -c:a libopus -b:a 192k";
        else if (q == 'q') params = "-c:v libvpx-vp9 -crf 35 -b:v 0 -deadline realtime -c:a libopus -b:a 96k";
        else               params = "-c:v libvpx-vp9 -crf 30 -b:v 0 -deadline good -c:a libopus -b:a 128k";
    } else if (fmt == "avi") {
        if (q == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a mp3 -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a mp3 -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a mp3 -b:a 160k";
    } else if (fmt == "wmv") {
        if (q == 'b')      params = "-c:v wmv2 -b:v 4M -c:a wmav2 -b:a 192k";
        else if (q == 'q') params = "-c:v wmv2 -b:v 1M -c:a wmav2 -b:a 96k";
        else               params = "-c:v wmv2 -b:v 2M -c:a wmav2 -b:a 128k";
    } else if (fmt == "flv") {
        if (q == 'b')      params = "-c:v libx264 -crf 19 -preset slow -c:a aac -b:a 160k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 96k";
        else               params = "-c:v libx264 -crf 23 -preset medium -c:a aac -b:a 128k";
    } else if (fmt == "mpeg-2") {
        if (q == 'b')      params = "-c:v mpeg2video -b:v 8M -maxrate 9M -bufsize 2M -c:a mp2 -b:a 224k";
        else if (q == 'q') params = "-c:v mpeg2video -b:v 2M -maxrate 3M -bufsize 1M -c:a mp2 -b:a 128k";
        else               params = "-c:v mpeg2video -b:v 5M -maxrate 6M -bufsize 2M -c:a mp2 -b:a 160k";
    } else if (fmt == "3gp" || fmt == "3g2") {
        if (q == 'b')      params = "-c:v h263 -b:v 500k -r 15 -s cif -c:a libopencore_amrnb -b:a 12.2k -ar 8000 -ac 1";
        else if (q == 'q') params = "-c:v h263 -b:v 150k -r 10 -s qcif -c:a libopencore_amrnb -b:a 4.75k -ar 8000 -ac 1";
        else               params = "-c:v h263 -b:v 300k -r 15 -s qcif -c:a libopencore_amrnb -b:a 7.4k -ar 8000 -ac 1";
    } else if (fmt == "avchd" || fmt == "mts" || fmt == "m2ts") {
        if (q == 'b')      params = "-c:v libx264 -crf 18 -preset slow -c:a ac3 -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a ac3 -b:a 128k";
        else               params = "-c:v libx264 -crf 23 -preset medium -c:a ac3 -b:a 160k";
    } else if (fmt == "ogv" || fmt == "ogg") {
        if (q == 'b')      params = "-c:v libtheora -q:v 7 -c:a libvorbis -q:a 6";
        else if (q == 'q') params = "-c:v libtheora -q:v 3 -c:a libvorbis -q:a 3";
        else               params = "-c:v libtheora -q:v 5 -c:a libvorbis -q:a 5";
    } else if (fmt == "prores") {
        if (q == 'b')      params = "-c:v prores_ks -profile:v 3 -vendor ap10 -pix_fmt yuv422p10le -c:a pcm_s16le";
        else if (q == 'q') params = "-c:v prores_ks -profile:v 0 -vendor ap10 -pix_fmt yuv422p10le -c:a pcm_s16le";
        else               params = "-c:v prores_ks -profile:v 2 -vendor ap10 -pix_fmt yuv422p10le -c:a pcm_s16le";
    } else if (fmt == "dnxhd" || fmt == "dnxhr") {
        if (q == 'b')      params = "-c:v dnxhd -profile:v dnxhr_hq -pix_fmt yuv422p -c:a pcm_s16le";
        else if (q == 'q') params = "-c:v dnxhd -profile:v dnxhr_lb -pix_fmt yuv422p -c:a pcm_s16le";
        else               params = "-c:v dnxhd -profile:v dnxhr_sq -pix_fmt yuv422p -c:a pcm_s16le";
    } else { //intentional repetition of the first if block, may change in the future
        if (q == 'b')      params = "-c:v libx264 -crf 17 -preset slow -c:a aac -b:a 192k";
        else if (q == 'q') params = "-c:v libx264 -crf 28 -preset superfast -c:a aac -b:a 128k";
        else               params = "-c:v libx264 -crf 22 -preset medium -c:a aac -b:a 160k";
    }

    std::string cmd = Program::Build::command("ffmpeg", session.safe_input(), session.safe_output(), params);
    Program::log("[-] Status: Executing Video Conversion: " + cmd + " [-]");
    Program::print("[~] Status: Converting Video... [~]\n");

    bool success = false; // error catching -_-
    if (int execute = std::system(cmd.c_str()); execute == -1) { // the system shell itself couldn't be started, critical
        std::cerr << "[!] Critical Error: Failed to initiate the command shell. [!]\n";
    } else { // did the command even finish normally
        #ifdef _WIN32
            if (execute == 0) {
                std::cout << "[~] Status: Conversion completed successfully! [~]" << std::endl;
                success = true;
            } else {
                std::cerr << " [!] Error: FFmpeg failed with exit code: " << execute << std::endl;
            }
        #else
            if (WIFEXITED(execute)) {
                if (int exitCode = WEXITSTATUS(execute); exitCode == 0) { //successful conversion
                    Program::print("[~] Status: Conversion completed successfully! [~]\n");
                    success = true;
                } else { // either the constructed cmd is wrong or FFmpeg is acting up
                    Program::print("[!] Error: FFmpeg failed with exit code: " + std::to_string(exitCode) + "[!]\n", Program::PrintType::Error);
                }
            } else { // so FFmpeg was terminated either by the user or the system itself(might have crashed)
                Program::print("[!] Error: FFmpeg was terminated abnormally. [!]\n", Program::PrintType::Error);
                Program::print("[~] If you believe this is a bug, please report it. [~]\n"
                               "[~] Run fconvert -h or --help for more instructions. [~]\n");
            }
        #endif
    }

    if (success) {
        if (session.commit(out)) {
            Program::print("[~] Status: Conversion completed successfully! [~]\n");
        } else {
            Program::print("[!] Error: Failed to safely export output file from sandbox. [!]\n", Program::PrintType::Error);
        }
    } else {
        Program::print("[!] Error: FFmpeg execution failed or terminated prematurely. [!]\n", Program::PrintType::Error);
    }
}

void video() {
    std::string name = Program::Get::input("Video filename: ");
    std::filesystem::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        Program::log("[!] Error: Path could not be resolved. [!]");
        return;
    }

    std::string fmt = Program::Get::input("Format: ", Program::Case::Lower);

    if (fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        Program::log("[~] Detected: " + fmt + "\nQuitting... [~]");
        Program::print("[!] Successfully stopped the conversion! [!]");
        return;
    }

    static const std::set<std::string> valid_video = {"mp4", "mov", "avi", "wmv", "flv", 
                                                      "f4v", "mkv", "webm", "3gp", "3g2", 
                                                      "m4v", "f4v", "mpeg-2", "avchd", "mts",
                                                      "m2ts", "ogv", "ogg", "prores", "dnxhd", 
                                                      "dnxhr"};
    if (valid_video.find(fmt) == valid_video.end()) {
        Program::print("\n[!] Format '" + fmt + "' is not supported or doesn't exist. [!]\n");
        std::cout << "Supported video formats include:\n"
                  << "MP4 (H.264), MOV, AVI, WMV, FLV, F4V, MKV, WebM, 3GP & 3G2,\n"
                  << "M4V, MPEG-2, AVCHD, MTS, M2TS, OGV, OGG, PRORES, DNXHD, DNXHR\n"
                  << "\n[-] If you believe this is a bug, make sure to report it. [-]\n";
        return;
    }

    std::string qual = Program::Get::input("Select Quality ([Q]uick, [D]efault, [B]est): ", Program::Case::Lower);
    if (qual == "quit" || qual == "exit" || qual == "cancel") {
        std::cout << "[!] Successfully stopped the conversion! [!]";
        return;
    }

    if (!qual.empty() && (qual[0] == 'q' || qual[0] == 'd' || qual[0] == 'b'))
        video_convert_logic(in, fmt, qual[0], false);
    else {
        Program::print("[!] Invalid quality option provided: " + qual + " [!]\nExiting...");
        return;
    }
}