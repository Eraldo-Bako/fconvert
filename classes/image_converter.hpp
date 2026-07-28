// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef IMAGE_CONVERTER_HPP
#define IMAGE_CONVERTER_HPP

#include <filesystem>
#include <string>

#include <opencv2/core.hpp> // #include <opencv2/opencv.hpp>

namespace Image {
    enum class SVG { VECTOR, CVECTOR, BIMGV, PIMGV, CUSTOM, INACTIVE, QUIT };
}

cv::Mat read_camera_raw(const std::string& raw_path);
void image_convert_logic(std::filesystem::path in, std::string fmt, bool silent, Image::SVG vector_precision = Image::SVG::INACTIVE);
void image();

#endif