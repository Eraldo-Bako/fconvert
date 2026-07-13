//
// Created by eraldo on 7/12/26.
//
#pragma once
#ifndef SECURE_CONVERSION_SESSION_HPP
#define SECURE_CONVERSION_SESSION_HPP
#include <filesystem>
#include <string>

class SecureConversionSession {
private:
    std::filesystem::path safe_input_path;
    std::filesystem::path safe_output_path;
    bool is_committed = false;

public:
    // Constructor: Safely stages the original file into an isolated environment
    SecureConversionSession(const std::filesystem::path& originalInput, const std::string& targetExtension);

    // Destructor: will automatically clean up whatever it made in the cache dir
    ~SecureConversionSession();

    // Getters for your safe parameters
    std::string safe_input()  const { return safe_input_path.string(); }
    std::string safe_output() const { return safe_output_path.string(); }

    // moves the safe output back to the user's real target output directory
    bool commit(const std::filesystem::path& finalUserDestination);
};
#endif