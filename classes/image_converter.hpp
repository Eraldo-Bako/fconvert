// fconvert v2.2.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef IMAGE_CONVERTER_HPP
#define IMAGE_CONVERTER_HPP

#include <filesystem>
#include <string>

#include <opencv2/core.hpp> // #include <opencv2/opencv.hpp>

#ifdef _WIN32
    #include <libraw.h>
#else
    #include <libraw/libraw.h>
#endif

cv::Mat read_camera_raw(const std::string& raw_path);
void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent);
void image();

#endif