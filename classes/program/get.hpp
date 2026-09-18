// fconvert v2.4.1-rc3 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#pragma once
#ifndef PROGRAM_GET_HPP
#define PROGRAM_GET_HPP

#include "../program_handler.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace Program {
    enum class Case { Normal, Lower, Upper }; // upper is unused atm, but it is there if I want to implement it
    enum class InputType { noWS, WS };

    namespace Get {
        std::string localeDirectory();
        std::filesystem::path logDirectory();
        std::string currentTimestamp();
        std::string input(const std::string& prompt, Case lower = Program::Case::Normal, InputType useWS = Program::InputType::WS);
        std::string toolPath(const std::string& baseToolName);

        struct ParsedInput {
            std::string path;
            std::string extension;
        };

        std::vector<ParsedInput> multipleInput(const std::string& fullInput);

        struct FileSignature {
            std::array<uint8_t, 16> magic;
            size_t offset;
            std::string extension;
        };

        /** old
         * std::string(std::filesystem::path) >> Program::Get::extensionFromHeader >> std::string
         * Reads the file through std::ifstream in std::ios::binary format
         * Reads a range of 1-4kb of header data to determine the filetype
         */

        inline std::string extensionFromHeader(const std::string& filePath) {

            std::error_code error_state;
            const auto file_size = std::filesystem::file_size(filePath, error_state);
            if (error_state) {
                Program::print(fmt::format(_("[!!] Error: {0} could not determine file size! [!!]"), "fconvert"), Program::PrintType::Error);
                Program::log(_("[!] Warning: Unsafe state detected. Stopping... [!]"));
                Program::end(fmt::format(_("[Error] Program stopped due to: {0}"), error_state.message()));
                return ""; //unreachable due to Program::end
            }
            if (file_size < 4) return "";
            
            //read only after confirming the file size
            std::ifstream file(filePath, std::ios::binary);
            if (!file.is_open()) return "";
            file.seekg(0, std::ios::beg);

            //initial buffer - 16bytes
            std::vector<uint8_t> buffer(16, 0);
            file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            const std::streamsize bytes_read = file.gcount();

            /*
            TODO: 
                  ai

                  pcm, dsd
                  alac, wavpack
                  opus, wma

                  prores, dnxhr, dnxhd
                  avchd

            
            */

            const static Program::Get::FileSignature signatures[] = {
                // IMAGE
                // STANDARD IMAGES - jpg/jpeg, png, webp, tiff/tif, bmp
                {{0xFF, 0xD8, 0xFF}, 0, ".jpg"}, // SOI + first APP0 byte (JFIF and EXIF)
                {{0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A}, 0, ".png"}, // 8-bit byte + PNG + CR LF(DOS) + DOS EOF + LF(Unix)
                {{0x57, 0x45, 0x42, 0x50}, 8, ".webp"},// 4 bytes starting at 8 gets *WEBP*, uses the RIFF container
                {{0x42, 0x4D}, 0, ".bmp"}, // BM*
                //{{0x49, 0x49, 0x2A, 0x00}, 0, ".tif"}, // little-endian
                //{{0x4D, 0x4D, 0x00, 0x2A}, 0, ".tif"}, // big-endian

                // Camera RAW - cr2, nef, arw, dng, crw
                {{0x43, 0x52, 0x02, 0x00}, 8, ".cr2"}, // CR + version 2.0
                {{0x48, 0x45, 0x41, 0x50}, 6, ".crw"}, // HEAP
                {{0x66, 0x74, 0x79, 0x70, 0x63, 0x72, 0x78, 0x33}, 4, ".cr3"}, // ftypcrx3, modern cr should add support
                {{0x46, 0x55, 0x4A, 0x49, 0x46, 0x49, 0x4C, 0x4D}, 0, ".raf"}, // FUJIFILM, full header is FUJIFILMCCD-RAW
                {{0x49, 0x49, 0x55, 0x00}, 0, ".rw2"}, // IIU\0

                // VECTOR/COMPOSITE - psd, svg, pdf, gif, ico, xcf, eps, ai
                {{0x3C, 0x3F, 0x78, 0x6D, 0x6C}, 0, ".svg"},
                {{0x25, 0x50, 0x44, 0x46}, 0, ".pdf"}, // checks only the first 4 bytes(%PDF)
                {{0x47, 0x49, 0x46, 0x38}, 0, ".gif"}, // GIF8* both animated(GIF89a) and not(GIF87a)
                {{0x00, 0x00, 0x01, 0x00}, 0, ".ico"},
                {{0x38, 0x42, 0x50, 0x53}, 0, ".psd"},
                {{0x25, 0x21, 0x50, 0x53, 0x2D, 0x41, 0x64, 0x6F}, 0, ".eps"}, // Encapsulated PostScript file
                {{0xC5, 0xD0, 0xD3, 0xC6}, 0, ".eps"}, // Adobe encapsulated PostScript
                {{0x67, 0x69, 0x6D, 0x70, 0x20, 0x78, 0x63, 0x66}, 0, ".xcf"},

                //  MODERN COMPRESSED - heic, heif
                {{0x68, 0x65, 0x69, 0x63}, 8, ".heic"}, // header is the same so just going to identify it as heic

                // AUDIO
                // UNCOMPRESSED - wav, aiff, pcm, dsd
                {{0x57, 0x41, 0x56, 0x45}, 8, ".wav"},// 4 bytes starting at 8 gets *WEBP*, uses the RIFF container
                {{0x46, 0x4F, 0x52, 0x4D, 0x00}, 0, ".aiff"},

                // LOSSLESS COMPRESSED - flac, alac, wavpack
                {{0x66, 0x4C, 0x61, 0x43}, 0, ".flac"},

                // LOSSY COMPRESSED - mp3, ogg, aac, m4a, opus, wma
                {{0x49, 0x44, 0x33}, 0, ".mp3"},
                {{0xFF, 0xFB}, 0, ".mp3"},       
                // {{0x4F, 0x67, 0x67, 0x53}, 0, ".ogg"},
                {{0xFF, 0xF1}, 0, ".aac"}, //MPEG-4 AAC
                {{0xFF, 0xF9}, 0, ".aac"}, //MPEG-2 AAC
                {{0x00, 0x00, 0x00, 0x20, 0x66, 0x74, 0x79, 0x70, 0x4D, 0x34, 0x41}, 0, ".m4a"}, //Apple audio and video
                {{0x66, 0x74, 0x79, 0x70, 0x4D, 0x34, 0x41, 0x20}, 4, ".m4a"}, //Apple Lossless Audio Codec file
                
                // VIDEO
                // COMPRESSED/DELIVERY - mp4, mkv, mov, m4v
                // dealing w/ mp4 later in the code
                //{{0x6D, 0x70, 0x34, 0x31}, 8, ".mp4"}, //mp41
                //{{0x6D, 0x70, 0x34, 0x32}, 8, ".mp4"}, //mp42
                //{{0x69, 0x73, 0x6F, 0x6D}, 8, ".mp4"}, //isom
                {{0x71, 0x74, 0x20, 0x20}, 8, ".mov"},
                {{0x66, 0x74, 0x79, 0x70, 0x4d, 0x34, 0x56}, 4, ".m4v"}, // ftypM4V

                // WEB OPTIMIZED - webm, ogg
                // webm later in the code
                // {{0x4F, 0x67, 0x67, 0x53}, 0, ".ogg"},

                // EDITING/INTERMEDIATE - prores, dnxhr, dnxhd
                // these are a big mess

                // HARDWARE/ACQUISITION: avchd, mpg, mpeg
                {{0x00, 0x00, 0x01, 0xBA}, 0, ".mpg"}, // MPEG-1/2 Part 1 (using mpg for this since its older)
                {{0x00, 0x00, 0x01, 0xB3}, 0, ".mpeg"}, // MPEG-1/2 Part 2

                // LEGACY - avi, wmv, flv, f4v, 3gp, 3g2
                {{0x41, 0x56, 0x49, 0x20}, 8, ".avi"}, // 4 bytes starting at 8 gets *WEBP*, uses the RIFF container
                {{0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11}, 0, ".wmv"}, //Windows Media Audio-Video File: either wma or wmv
                
                {{0x46, 0x4C, 0x56}, 0, ".flv"}, // FLV
                {{0x66, 0x74, 0x79, 0x70, 0x66, 0x34, 0x76}, 4, ".f4v"}, // ftypf4v

                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x31}, 4, ".3gp"}, // ftyp3gp1
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x32}, 4, ".3gp"}, // ftyp3gp2
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x33}, 4, ".3gp"}, // ftyp3gp3
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x34}, 4, ".3gp"}, // ftyp3gp4
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x35}, 4, ".3gp"}, // ftyp3gp5
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x36}, 4, ".3gp"}, // ftyp3gp6
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x70, 0x37}, 4, ".3gp"}, // ftyp3gp7
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x32, 0x61}, 4, ".3g2"}, // ftyp3g2a
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x32, 0x62}, 4, ".3g2"}, // ftyp3g2b
                {{0x66, 0x74, 0x79, 0x70, 0x33, 0x67, 0x32, 0x63}, 4, ".3g2"}  // ftyp3g2c
            };

            // Preset Signatures check
            for (const auto& sig : signatures) {
                if (bytes_read < sig.offset + sig.magic.size()) continue;
                if (std::equal(sig.magic.begin(), sig.magic.end(), buffer.begin() + sig.offset)) {
                    return sig.extension;
                }
            }

            // Dynamic MP4 brand verification (requires at least 12 bytes read)
            if (bytes_read >= 12) {
                const std::string_view ftyp{reinterpret_cast<const char*>(buffer.data() + 4), 4};
                const std::string_view brand{reinterpret_cast<const char*>(buffer.data() + 8), 4};

                if (ftyp == "ftyp") {
                    static constexpr std::array<std::string_view, 6> mp4_brands{
                        "isom", "mp41", "mp42", "MSNV", "avc1", "3gp5"
                    };

                    if (std::find(mp4_brands.begin(), mp4_brands.end(), brand) != mp4_brands.end()) {
                        return ".mp4";
                    }
                }
            } file.clear();
            
            { // reading up to 36 bytes for ogg and opus distinction
                file.seekg(0, std::ios::beg);
                const size_t bytes_to_read = std::min<size_t>(
                                                static_cast<size_t>(file_size), 
                                                36
                                            );
                std::vector<char> opus_buffer(bytes_to_read);
                file.read(opus_buffer.data(), bytes_to_read);
                const size_t opus_bytes_read = static_cast<size_t>(file.gcount());
                if (opus_bytes_read == 0) return "";
                if (opus_bytes_read >= 4 && opus_buffer[0] == 0x4F && opus_buffer[1] == 0x67 && opus_buffer[2] == 0x67 && opus_buffer[3] == 0x53) {
                    if(opus_bytes_read >= 36 && 
                        opus_buffer[28] == 0x4f && opus_buffer[29] == 0x70 && 
                        opus_buffer[30] == 0x75 && opus_buffer[31] == 0x73 &&
                        opus_buffer[32] == 0x48 && opus_buffer[33] == 0x65 && 
                        opus_buffer[34] == 0x61 && opus_buffer[35] == 0x64
                    ) {
                        return ".opus";
                    }
                    return ".ogg";
                }
            } file.clear();

            // Container Deep Inspection (reads up to 4KB)
            file.seekg(0, std::ios::beg);
            const size_t bytes_to_read = std::min<size_t>(
                                            static_cast<size_t>(file_size), 
                                            4096
                                        );
            std::vector<char> deep_buffer(bytes_to_read);
            
            file.read(deep_buffer.data(), bytes_to_read);
            const size_t deep_bytes_read = static_cast<size_t>(file.gcount());

            if (deep_bytes_read == 0) return "";
            std::string_view content(deep_buffer.data(), deep_bytes_read);
            
            // ZIP Container Check
            if (bytes_read >= 4 && buffer[0] == 0x50 && buffer[1] == 0x4B && buffer[2] == 0x03 && buffer[3] == 0x04) {
                if (content.find("application/epub+zip") != std::string_view::npos) return ".epub";
                if (content.find("word/") != std::string_view::npos || content.find("[Content_Types].xml") != std::string_view::npos) return ".docx";
                return ".zip";
            }

            // TIFF Container Check for RAW files
            if (bytes_read >= 4 && (
                (buffer[0] == 0x49 && buffer[1] == 0x49 && buffer[2] == 0x2A && buffer[3] == 0x00) ||
                (buffer[0] == 0x4D && buffer[1] == 0x4D && buffer[2] == 0x00 && buffer[3] == 0x2A)
            )) {
                
                if (content.find("NIKON") != std::string_view::npos)   return ".nef";
                if (content.find("Sony") != std::string_view::npos)    return ".arw";
                if (content.find("PENTAX") != std::string_view::npos)  return ".pef";
                if (content.find("OLYMPUS") != std::string_view::npos) return ".orf";

                // Searching for DNGVersion tag ID 50706
                bool is_little_endian = (buffer[0] == 0x49);
                std::string_view dng_sig = is_little_endian 
                    ? std::string_view("\x12\xC6\x01\x00\x04\x00\x00\x00", 8)   // LE: 12 c6 01 00 04 00 00 00
                    : std::string_view("\xC6\x12\x00\x01\x00\x00\x00\x04", 8);  // BE: c6 12 00 01 00 00 00 04
                if (content.find(dng_sig) != std::string_view::npos)   return ".dng";

                return ".tif";
            }

            // EBML / Matroska Check (MKV vs WEBM)
            if (bytes_read >= 4 && buffer[0] == 0x1A && buffer[1] == 0x45 && buffer[2] == 0xDF && buffer[3] == 0xA3) {
                if (content.find("webm") != std::string_view::npos) return ".webm";
                return ".mkv";
            }

            return "";
        }
    }
}

#endif

/* DEVINFO
 * I used `hexdump` from gnu/linux-utils to read the files and determine the byte signatures:
 * $ hexdump -C test.extension | head
 * 00000000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00 |................|
**/
