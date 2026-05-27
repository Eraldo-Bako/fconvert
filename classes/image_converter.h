// fconvert v2.1.0 (c) 2023 - 2026 Eraldo Bako - MIT License
// Maintaier: eraldobako@gmail.com
#ifndef IMAGE_CONVERTER_H
#define IMAGE_CONVERTER_H

#include <filesystem>
#include <string>
#include <libraw/libraw.h>
#include <opencv2/opencv.hpp>

cv::Mat read_camera_raw(const std::string& raw_path);
void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent);
void image();

#endif