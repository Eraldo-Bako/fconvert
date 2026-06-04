# Changelog
### Patch Notes v2.2.0
```
maintainer : Eraldo Bako;
email : <eraldobako@gmail.com>;
package : fconvert;
version : 2.1.1;
name-me-pls: alloy;
license: MIT, CC BY-NC-ND(branding only);
Features:
* Improved included libraries in header and class files for smaller file-size.
* Added better ebook/document conversion logic
* Small improvements and fixes.
* Using .hpp extension hor header files instead of .h, so the compiler always treats them like Cpp header files
```
### Patch Notes v2.1.1
```
maintainer : Eraldo Bako;
email : <eraldobako@gmail.com>;
package : fconvert;
version : 2.1.1;
name-me-pls: smile;
license: MIT, CC BY-NC-ND(branding only);
Fixed bugs :
* Fixed defining libraries for Windows
* Fixed Error catching for Windows
* Added CMakeLists.txt fallback for Windows
* Improved build command in PKGBUILD
```
### Patch Notes v2.1.0
```
maintainer : Eraldo Bako;
email : <eraldobako@gmail.com>;
package : fconvert;
version : 2.1.0;
name-me-pls: stuff;
license: MIT, CC BY-NC-ND(branding only);
Fixed bugs :
* Quitting Bug when reaching EOF as input
Features :
* Added support for RAW image formats as input only
* Added new method in image_converter.cpp for making RAW images accessible by OpenCV through LibRaw:
    cv::Mat read_camera_raw(const std::string& raw_path)
* Added external_write boolean when using ImageMagick
* Added support for project-type image formats(psd, xcf, ai, pdf, etc) using ImageMagick and Ghostscript
* Added better support for libheif libde265 x265 codecs
* Added better support for overall image, video, and audio conversions
    - Images: using better OpenCV methods for reading and conversion
    - Video: using better FFmpeg flags and presets for Quick, Default, and Best for conversion
    - Audio: using better FFmpeg flags and audio specific presets
* Added eBook support for the -f flag
* Added icon.svg, icon.ico that is used as part of the fconvert branding
* Improved CMakeLists.txt logic for better Windows support
* Added option to build without LibRaw if not preferred while building using CMake
* Using resources.rc to embed an icon when building the Windows binary
Comment : "Read more in [PROGRESS.md](./PROGRESS.md)"
```
### Patch Notes v2.0.3
```
maintainer : Eraldo Bako;
email : <eraldobako@gmail.com>;
package : fconvert;
version : 2.0.3;
name-me-pls: vocal;
license: MIT, CC BY-NC-ND(branding only);
Fixed bugs :
* Quitting Bug when providing "ENTER" as input
* fconvert -f defaulting to interactive mode when no file path or extension falg provided
* Debug Mode no longer just a framework
* Small fixes in README.md
Features :
* Fully interactive Debug Mode. Logs will appear as [DEBUG] ------ [`] {Proccess}: {Info} [`]
* Improved Case-Sensitivity(safe conversion to lowercase) and Input Buffer Skipping
* Introduced a possible framework for ebook conversion(highly experimental):
    - Not functional with the -f flag
    - Visually hidden as an option, unless programm is running in debug mode.
    - Kind of supporting PDF, DOCX, EPUB, HTML, TXT
* License Updated:
    - Copyright (c) 2023 - 2026 Eraldo Bako
    - Added year 2023 as this is when this project was first published
* PKGBUILD using CMakeLists.txt and using gcc command as fallback
* Added maintainer info and license disclaimer in file containing source code
* Added PathHandler::build_pandoc_cmd method and changed the PathHandler::get_clean_cmd method for FFmpeg command construction to PathHandler::build_ffmpeg_cmd
* Added support for .<ext> when usinf -f flag:
    - Before: $ fconvert -f file.png -extension
    - Added:  $ fconvert -f file.png .extension
    - users can either use the -<ext> or .<ext> format
Comment : "Read more in [PROGRESS.md](./PROGRESS.md)"
```
### Patch Notes v2.0.2
```
maintainer : Eraldo Bako;
email : <eraldobako@gmail.com>;
package : fconvert;
version : 2.0.2;
name-me-pls: quack;
license: MIT, CC BY-NC-ND(branding only);
Fixed bugs :
* Fixed case-sensitivity logic in audio, video, and image format validation,
* Improved input buffer handling to prevent menu skipping,
* Switched to `std::set` for more format checking;
Features:
* Added `-v` and `--version` flags,
* Introduced `interactive_mode()` with a new ASCII art banner and new menus,
* Placeholder `ebook()` function for future expansion;
```
### Patch Notes v2.0.1
```
maintainer : Eraldo Bako;
email : <eraldobako@gmail.com>;
package : fconvert;
version : 2.0.1;
name-me-pls: eerie;
license: MIT;
Fixed bugs :

* Error while running the installed package system-wide on random path without input/output folders.

$ fconvert -f example.png -webp
terminate called after throwing an instance of 'std::filesystem::__cxx11::filesystem_error'
  what():  filesystem error: cannot create directories: Invalid argument []
Aborted                    (core dumped) fconvert -f example.png -webp

Used to need the full path - it should also look in the working directory
$ fconvert -f /home/user/Pictures/example.png -webp
 [+] Saved: "example.webp"

Now, if package installed system-wide, it will also look in the working directory instead of requiring the full path.
$ fconvert -f example.png -webp
 [+] Saved: "example.webp
$ fconvert -f /home/user/Pictures/example.png -webp
 [+] Saved: "example.webp

-----------------------------------------

* Corrupt AAC audio conversion - the final output lacked right time stamps.
Before it was being converted by:
//lines 22-24
else {
    params = "-c:a aac -b:a 192k";
}

This conversion lacked the right flags and was the final 'catch-all' block.

The first logic-fix would add the aac conversion to m4a, since it already serves as a/an container/extension.
Flags:
    -movflags +faststart: Moves metadata to the front (fixes timestamps),
    -bsf:a aac_adtstoasc: Ensures the bitstream is container-friendly:

else if (fmt == "m4a" || fmt == "aac") {
    params = "-c:a aac -b:a 256k -movflags +faststart -bsf:a aac_adtstoasc";
}

This still didn't fully address the metadata missmatch which causes players to estimate the duration of the track.
I noticed that this happened only when using an extracted audio from a video.
Therefore, I divided m4a and aac conversion operations into separate blocks:
Flags: 
    -fflags +genpts: Rebuilds the timeline from scratch
    -vn -sn -dn: Strips out video/subtitle junk
    -af \"aresample=async=1\": Rebuilds the timeline from scratch.

else if (fmt == "m4a") {
    params = "-c:a aac -b:a 256k -movflags +faststart";
} else if (fmt == "aac") {
    params = "-fflags +genpts -vn -sn -dn -c:a aac -b:a 256k -af \"aresample=async=1\"";
}

The FFMPEG wiki recommended a libray only available on the paid version(libfdk_aac).
While these flags won't provide the cleanest aac file, the audio file is completely fine, playable, and fully recognised by editing software.

Also improved on the 'catch-all' else block by adding -ar 44100, ensuring a standard sampling rate if the format is unknown
else {
    params = "-c:a aac -b:a 192k -ar 44100";
};
Features:
* Improved path_handler logic
* Improved aac conversion support;
```
### Patch Notes v2.0.0
```
* Rewritten overall logic.
* Name change to fconvert.
* Package added to the AUR.
```
### Patch Notes v1.0.0
```
* Initial release of file-converter
```
**EOF**