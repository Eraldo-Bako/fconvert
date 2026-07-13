//
// Created by eraldo on 7/12/26.
//
#include "secure_conversion_session.hpp"
#include "program_handler.hpp"

#include <filesystem>
#include <string>
#include <atomic>
#include <iostream>

//stages the original file into an isolated environment
SecureConversionSession::SecureConversionSession(
    const std::filesystem::path& originalInput,
    const std::string& targetExtension) {
    try {

        std::filesystem::path tempDir = Program::Get::logDirectory() / "cache";
        if (!std::filesystem::exists(tempDir)) std::filesystem::create_directories(tempDir);

        //generate name containing ONLY safe characters - for future multi-conversion feature
        static std::atomic<uint64_t> session_id{0};
        std::string idStr = std::to_string(session_id.fetch_add(1));

        std::string inExt = originalInput.extension().string();
        std::string outExt = (targetExtension.empty() || targetExtension.front() == '.') ? targetExtension : "." + targetExtension;

        safe_input_path = tempDir / ("stage_in_" + idStr + inExt);
        safe_output_path = tempDir / ("stage_out_" + idStr + outExt);

        std::filesystem::copy_file(originalInput, safe_input_path, std::filesystem::copy_options::overwrite_existing);

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[fconvert Error] - Security Staging Failed: " << e.what() << "\n";
        throw;
    }
}

SecureConversionSession::~SecureConversionSession() {
    try {
        if (std::filesystem::exists(safe_input_path)) std::filesystem::remove(safe_input_path);
        if (std::filesystem::exists(safe_output_path)) std::filesystem::remove(safe_output_path);
    } catch (...) {
        // TODO: Suppress exceptions in destructors to prevent termination
        // in the future
    }
}

//moves the safe output back to the user's real target output directory
bool SecureConversionSession::commit(const std::filesystem::path& finalUserDestination) {
    if (!std::filesystem::exists(safe_output_path)) return false;
    try {
        std::filesystem::copy_file(safe_output_path, finalUserDestination, std::filesystem::copy_options::overwrite_existing);
        is_committed = true;
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[!] Failed to export converted file: " << e.what() << "\n";
        return false;
    }
}