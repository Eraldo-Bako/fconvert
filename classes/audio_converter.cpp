// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "audio_converter.hpp"
#include "path_handler.hpp"
#include "program_handler.hpp"
#include "secure_conversion_session.hpp"

#include <iostream>
#include <set>
#include <algorithm>
#include <fmt/core.h>

#ifndef _WIN32
    #include <sys/wait.h>
#endif

void audio_convert_logic(std::filesystem::path in, std::string fmt, bool silent) {

    if (!Program::Check::ffmpeg()) {
        Program::print(_("[!] Error: FFmpeg not found. [!]\n"), Program::PrintType::Error);
        return;
    }

    // checking the existence of the input_file and the directory
    // defining the location and handling conflicts for the output_file
    std::filesystem::path out = PathHandler::handle_conflicts(PathHandler::get_output_path(in, "." + fmt), silent);
    if (out.empty()) return;

    // isolating the original input into a shell-safe path
    SecureConversionSession session(in, fmt);

    // varied on the format, defining the best parameters to fetch to the FFmpeg command
    std::string params;
    Program::log(_("[~] Status: Defining conversion parameters. [~]"));
    // UNCOMPRESSED AUDIO
    if (fmt == "wav") {
        params = "-c:a pcm_s16le";
    } else if (fmt == "aiff") {
        params = "-c:a pcm_s16be";
    } else if (fmt == "pcm") {
        params = "-f s16le -c:a pcm_s16le";
    } else if (fmt == "dsd") {
        params = "-c:a dsd_lsbf_planer"; 
    } // LOSSLESS COMPRESSED AUDIO
    else if (fmt == "flac") {
        params = "-c:a flac";
    } else if (fmt == "alac") {
        params = "-c:a alac";
    } else if (fmt == "wavpack") {
        params = "-c:a wavpack";
    } // LOSSY COMPRESSED AUDIO
    else if (fmt == "mp3") {
        params = "-c:a libmp3lame -q:a 0";
    } else if (fmt == "ogg") {
        params = "-c:a libvorbis -q:a 5";
    } else if (fmt == "m4a") {
        params = "-c:a aac -b:a 256k -movflags +faststart";
    } else if (fmt == "aac") { // aac is annoying, why do yall even use it
        params = "-fflags +genpts -vn -sn -dn -c:a aac -b:a 256k -af \"aresample=async=1\"";
    } else if (fmt == "opus") { // the best for penguin, use it always
        params = "-c:a libopus -b:a 128k";
    } else if (fmt == "wma") { // ms legacy artifact for gramps, gross
        params = "-c:a wmav2 -b:a 192k";
    } else { // --- FALLBACK ---
        params = "-c:a aac -b:a 192k -ar 44100";
    }

    // constructs the FFmpeg command and then executes
    std::string cmd = Program::Build::command(Program::Build::cmdType::FFmpeg, session.safe_input(), session.safe_output(), params);
    Program::log(fmt::format(_("[-] Executing Audio Conversion: {} [-]"), cmd));
    Program::print(_("[~] Status: Converting Audio... [~]\n"));

    bool success = false; // error catching -_-
    if (int execute = std::system(cmd.c_str()); execute == -1) {
        // the system shell itself couldn't be started
        std::cerr << _("[!] Critical Error: Failed to initiate the command shell. [!]") << std::endl;
    } else { // did the command even finish normally
        #ifdef _WIN32
            int exitCode = execute;
            if (exitCode == 0) {
                Program::print(_("[~] Status: Conversion completed successfully! [~]\n"));
                success = true;
            } else {
                Program::print(fmt::format(_("[!] Error: FFmpeg failed with exit code: {0} [!]\n"), std::to_string(exitCode)), Program::PrintType::Error);
            }
        #else
            if (WIFEXITED(execute)) {
                int exitCode = WEXITSTATUS(execute);
                if (exitCode == 0) { //successful conversion
                    Program::print(_("[~] Status: Conversion completed successfully! [~]\n"));
                    success = true;
                } else { // either the constructed cmd is wrong or FFmpeg is acting up
                    Program::print(fmt::format(_("[!] Error: FFmpeg failed with exit code: {0} [!]\n"), std::to_string(exitCode)), Program::PrintType::Error);
                }
            } else { // so FFmpeg was terminated either by the user or the system itself(might have crashed)
                Program::print(_("[!] Error: FFmpeg was terminated abnormally. [!]\n"), Program::PrintType::Error);
                Program::print(_("[~] If you believe this is a bug, please report it. [~]\n"
                               "[~] Run fconvert -h or --help for more instructions. [~]\n"));
            }
        #endif
    }

    if (success) { // Move file out of cache to the actual directory with conflict handler name
        if (session.commit(out)) {
            Program::print(_("[~] Status: Conversion completed successfully! [~]\n"));
        } else {
            Program::print(_("[!] Error: Failed to safely export output file from sandbox. [!]\n"), Program::PrintType::Error);
        }
    } else {
        Program::print(_("[!] Error: FFmpeg execution failed or terminated prematurely. [!]\n"), Program::PrintType::Error);
    }
}

void audio() {

    std::string name = Program::Get::input(_("Audio filename or path: "));
    std::filesystem::path in = PathHandler::resolve_input(name);
    if (in.empty()) {
        Program::log(_("[!] Error: Path could not be resolved. [!]"));
        return;
    }

    // fetch me the desired format w/ safe lowercase conversion
    std::string fmt = Program::Get::input(_("Format: "), Program::Case::Lower);

    // why u quitting +@+   jk, just providing a quiting option at anytime(promise u'll come back Q-Q)
    if (fmt == "q" || fmt == "quit" || fmt == "exit" || fmt == "cancel") {
        Program::log(fmt::format(_("[~] Detected: '{}' Quitting... [~]"), fmt));
        Program::print(_("[!] Successfully stopped the conversion! [!]"));
        return;
    }
    // seems like you either don't know how to read or write
    static const std::set<std::string> valid_audio = {"mp3", "wav", "flac", "aac", "ogg", "m4a", 
                                                      "wma", "opus", "aiff", "pcm", "dsd", "alac",
                                                      "wavpack"};
    if (valid_audio.find(fmt) == valid_audio.end()) {
        Program::log(_("[~] Detected: The provided format is incorrect. [~]"));
        std::cout << fmt::format(_("\n[!] Format '{}' is not supported or doesn't exist. [!]\n"), fmt)
                  << _("Supported audio formats include:\n")
                  << _("* Uncompressed: WAV, AIFF, PCM, DSD\n")
                  << _("* Lossless Compressed: FLAC, ALAC, WAVPACK\n")
                  << _("* Lossy Compressed: MP3, AAC, OGG, M4A, WMA, OPUS\n")
                  << _("\n[-] If you believe this is a bug, make sure to report it. [-]\n");
        return;
    } // actual conversion logic
    audio_convert_logic(in, fmt, false);
}