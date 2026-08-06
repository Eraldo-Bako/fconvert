<!-- fconvert v2.4.1 | Copyright (c) 2023-2026 Eraldo Bako -->
<!-- Licensed under the Apache License, Version 2.0 (the "License") -->
<!-- Maintainer: eraldobako@gmail.com -->

<div id="fconvert-logo" align="center">
    <br />
    <img src="./branding/icon.ico" alt="fconvert Logo" width="200"/>
    <h1>fconvert</h1>
    <h3>A fast CLI converter for Images, Videos, Audios, and Ebooks</h3>
</div>

**fconvert** is a fast, intentional, and hardware-agnostic C++ command-line utility. Designed for Arch Linux and other Unix-like systems, it provides a seamless way to convert images, videos, and audio files using a smart "Sandbox" directory logic.

---

## Key Features

* **Smart Routing:** Automatically detects intent. Extract audio from a video by running: `fconvert -f movie.mp4 -mp3`.
* **Sandbox Workflow:** Automatically monitors the `input_file/` directory and outputs to `output_file/` directory to keep your system clean.
* **Conflict Resolution:** Handles existing files gracefully (auto-renaming or prompt-to-overwrite).
* **CLI & Interactive:** Use rapid flags for automation or the guided menu for ease of use.
* **Hardware Agnostic:** Optimized for high-quality CPU encoding, ensuring portability across different environments.

---

## Supported Formats

<table>
  <thead>
    <tr>
      <th>Type</th>
      <th>Category</th>
      <th>Extensions</th>
      <th>Operation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td rowspan="4"><b>IMAGES</b></td>
      <td><b>STANDARD IMAGES</b></td>
      <td><code>jpg</code>, <code>jpeg</code>, <code>png</code>, <code>webp</code>, <code>tiff</code>, <code>bmp</code></td>
      <td>OpenCV</td>
    </tr>
    <tr>
      <td><b>CAMERA RAW</b></td>
      <td><code>cr2</code>, <code>nef</code>, <code>arw</code>, <code>dng</code>, <code>crw</code></td>
      <td>LibRAW + OpenCV</td>
    </tr>
    <tr>
      <td><b>VECTOR/COMPOSITE</b></td>
      <td><code>psd</code>, <code>svg</code>, <code>pdf</code>, <code>gif</code>, <code>ico</code>, <code>xcf</code>, <code>eps</code>, <code>ai</code></td>
      <td>ImageMagick + Ghostscript</td>
    </tr>
    <tr>
      <td><b>MODERN COMPRESSED</b></td>
      <td><code>heic</code>, <code>heif</code></td>
      <td>ImageMagick + OpenCV</td>
    </tr>
    <tr>
      <td rowspan="3"><b>AUDIO</b></td>
      <td><b>UNCOMPRESSED</b></td>
      <td><code>wav</code>, <code>aiff</code>, <code>pcm</code>, <code>dsd</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>LOSSLESS COMPRESSED</b></td>
      <td><code>flac</code>, <code>alac</code>, <code>wavpack</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>LOSSY COMPRESSED</b></td>
      <td><code>mp3</code>, <code>ogg</code>, <code>aac</code>, <code>m4a</code>, <code>opus</code>, <code>wma</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td rowspan="5"><b>VIDEO</b></td>
      <td><b>COMPRESSED/DELIVERY</b></td>
      <td><code>mp4</code>, <code>mkv</code>, <code>mov</code>, <code>m4v</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>WEB OPTIMIZED</b></td>
      <td><code>webm</code>, <code>ogg</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>EDITING/INTERMEDIATE</b></td>
      <td><code>prores</code>, <code>dnxhr</code>, <code>dnxhd</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>HARDWARE/ACQUISITION</b></td>
      <td><code>avchd</code>, <code>mpeg2</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>LEGACY</b></td>
      <td><code>avi</code>, <code>wmv</code>, <code>flv</code>, <code>f4v</code>, <code>3gp</code>, <code>3g2</code></td>
      <td>FFmpeg</td>
    </tr>
    <tr>
      <td><b>eBOOK</b></td>
      <td><b>DOCUMENTS</b></td>
      <td><code>pdf</code>, <code>epub</code>, <code>html</code>, <code>txt</code>, <code>docx</code></td>
      <td>Pandoc</td>
    </tr>
  </tbody>
</table>

> **Note:** Inputting a video file with an audio target extension (e.g.: `.mp4` to `-mp3`) will automatically trigger a high-quality audio extraction.

---

## Installation

### Prerequisites
Ensure you have the following installed on your system:

Build Dependencies:
* **C++17** compatible compiler (GCC/Clang)
* **FFmpeg** (for video and audio processing)
* **OpenCV 4+** (for image processing)

Runtime Dependencies (optional):
* **LibRaw** (for RAW image processing)
* **ImageMagick** (for vector/composite)
* **Ghostscript** (for vector/composite)
* **Pandoc** (for experimental document processing)
* **Typst**, **Weasyprint**, or **Xelatex** (as a PDF engine)

### Installation (Universal)
Download the latest release in the Releases Page of the repository.
- Linux: Download `fconvert-vX.X.X-x86_64.AppImage` (no dependencies required, contains the full program)
- Windows: Download `fconvert-vX.X.X-windows-x86_64.zip` (no dependencies required, contains the full program)
- macOS: There is no official release of fconvert for macOS. Try building it manually following the build instructions below.

### Installation (Arch Linux)
Clone the repo and use `makepkg` to install system-wide:
```bash
$ git clone https://github.com/Eraldo-Bako/fconvert
$ cd fconvert
$ makepkg -si
```
> **Note:** You could use a package helper like ```yay``` or ```paru``` since the package is in the AUR: ```yay -S fconvert``` or ```paru -S fconvert```
## Usage
> **Note:** If you installed the package system-wide, you can simply use `fconvert`. If you are running the binary locally from the build folder, use `./fconvert`.
### Quick Convert (CLI Mode)
Convert a file instantly without entering the menu:

```bash
fconvert -f image.png -webp
fconvert -f music.wav -flac
fconvert -f video.mp4 -mov
fconvert -f video.mp4 -mp3  # Automatically extracts high-quality audio
fconvert -f video.mp4 .mp3  # Valid form, added in version 2.0.3
```
### Interactive Mode
Simply run fconvert in your terminal or the executable to enter the guided interface:
```fconvert
$ fconvert
   __                              _   
  / _|                            | |  
 | |_ ___ ___  _ ____   _____ _ __| |_ 
 |  _/ __/ _ \| '_ \ \ / / _ \ '__| __|
 | || (_| (_) | | | \ V /  __/ |  | |_ 
 |_| \___\___/|_| |_|\_/ \___|_|   \__|

 ---------------------------- File Converter ----------------------------
 ----- A fast CLI converter for Images, Videos, Audios, and Ebooks! -----
 ------------------------------------------------------------------------
    
Convert [I]mage / [V]ideo / [A]udio / [E]book / [Q]uit: 

```
### Sandbox Logic
1. Place your files in the input_file/ folder.
2. Run fconvert.
3. Find your processed files in the output_file/ folder.

## Flags
* ```-v```, ```--version``` : Show package version and licensing information.
* ```-h```, ```--help``` : Show the help message and supported formats.
* ```-l```, ```--list``` : Show a list of the supported formats.
* ```-d```, ```--debug```: Enable debug mode for detailed path-resolution logging.
* ```-cc```, ```--clear-cache```: Clear the saved cache and log files of fconvert
<br></br>
* ```-f <file> -<ext>```: Execute a quick conversion.
* ```-f <file> .<ext>```: Alternatively, using `.<ext>` is a valid usage.
  <br></br>
* ```-i```, ```-intr```, ```-interactive```: Explicitly start in CLI interactive mode.

### Dependency-Hell
#### Linux: Arch Based
```bash
# Build:
$ sudo pacman -Syyu base-devel cmake ffmpeg opencv

# Runtime:
$ sudo pacman -Syyu libraw imagemagick ghostscript libheif libde265 x265
$ sudo pacman -Syyu pandoc typst 
$ yay -S vtracer
```
> NOTE: For Arch-based distributions typst is recommended, but you can use:<br>
> **Weasyprint**: `$ sudo pacman -S python-weasyprint`<br>
> **Xelatex**: `$ sudo pacman -S texlive-xetex`
#### Linux: Fedora Based
```bash
# Build:
$ sudo dnf check-update && sudo dnf upgrade
$ sudo dnf install @development-tools && sudo dnf install cmake ffmpeg-free opencv-devel

# Runtime:
$ sudo dnf install LibRaw-devel ImageMagick ghostscript libheif-devel libde265-devel x265-devel
$ sudo dnf install pandoc weasyprint
```
> NOTE: If you want to use typst as the PDF engine, for Fedora-based distributions, use copr:
>
> Enable the community Copr repository: `$ sudo dnf copr enable -y claaj/typst`
>
> Install via DNF normally: `$ sudo dnf install typst`
#### Linux: Debian Based(Ubuntu)
```bash
# Build:
$ sudo apt update && sudo apt install build-essential cmake ffmpeg libopencv-dev

# Runtime:
$ sudo apt install libraw-dev imagemagick ghostscript libheif-dev libde265-dev libx265-dev
$ sudo apt install pandoc weasyprint
```
#### Windows (winget for tools, vcpkg for libraries)
```powershell
# Build:
> winget install Microsoft.VisualStudio.2022.BuildTools --override "--add Microsoft.VisualStudio.Workload.VCTools"
> winget install Kitware.CMake
> vcpkg install ffmpeg opencv4

# Runtime:
> vcpkg install libraw graphicsmagick libheif libde265 x265
> winget install JohnMacFarlane.Pandoc 
> winget install Typst.Typst
> winget install Rustlang.Rustup
> cargo install vtracer
```
> NOTE: If you installed the optional packages(graphicsmagick/imagemagick), get and install ghostscript for Windows: [gs00000w64.exe](https://github.com/artifexsoftware/ghostpdl-downloads/releases)
#### macOS
```zsh
# Build (Requires Xcode Command Line Tools installed):
$ xcode-select --install
$ brew install cmake ffmpeg opencv

# Runtime:
$ brew install libraw imagemagick ghostscript libheif libde265 x265
$ brew install pandoc typst
```
### Build Instructions
For universal compatibility use CMake:
```bash
$ cd fconvert
$ cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr -DWITH_LIBRAW=ON 
# set -DWITH_LIBRAW=OFF if not needed
$ cmake --build build --config Release
```
Windows Exclusive - If you used vcpkg to install dependencies, these will be your build instructions:
```powershell
> cd fconvert
# Replace PATH_TO_VCPKG with your actual vcpkg path
> PATH_TO_VCPKG\vcpkg.exe install --triplet x64-windows
> cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="PATH_TO_VCPKG/scripts/buildsystems/vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows -DWITH_LIBRAW=ON
> cmake --build build --config Release
```
> **Note:** If you built it locally using CMake, the executable will be inside the `build/` directory (run with `./build/fconvert` on Linux/macOS or `.\build\Release\fconvert.exe` on Windows).

Building with GCC - For the majority of up-to-date Linux distributions, use one of the following g++ commands:

```bash
$ cd fconvert
# Linux
$ g++ -std=c++17 -O3 -s main.cpp classes/*.cpp classes/program/*.cpp -o fconvert $(pkg-config --cflags --libs opencv4 libraw) $CXXFLAGS $LDFLAGS
# Arch Based
$ g++ -std=c++17 -O3 -s main.cpp classes/*.cpp classes/program/*.cpp -o fconvert $(pkg-config --cflags --libs opencv5 libraw) $CXXFLAGS $LDFLAGS
# omit libraw if not needed
```

## Contributing
Contributions are welcome! If you find a bug or have a feature request, please open an issue first. 

* **Code Contributions:** Review our [CONTRIBUTING.md](./CONTRIBUTING.md) guide before submitting pull requests.
* **Translation Contributions:** Localization edits must be submitted via our [Hosted Weblate project](https://hosted.weblate.org/fconvert) rather than direct pull requests.

Before contributing, you are expected to have read, understood, and agreed to follow our [Guidelines](./CONTRIBUTING.md) and [Code of Conduct](./CODE_OF_CONDUCT.md).

## License
* **Source Code & Localization (`./po/`):** [Apache License, Version 2.0](LICENSE)
* **Branding & Logos (`./branding/`):** [CC BY-NC-ND 4.0](branding/LICENSE)
* **Official Binary Releases:** Distributed under [Apache License, Version 2.0](LICENSE)

> **Licensing & Brand Summary:** The core application, source code, and localization files in `./po/` are licensed under **Apache 2.0**. Official branding, logos, and visual graphics in `./branding/` are licensed under **CC BY-NC-ND 4.0**. Pursuant to Section 6 of the Apache License 2.0 and our branding terms, third-party forks or derivative distributions must rebrand and scrub protected project identifiers prior to distribution. For complete module breakdowns and third-party software notices, see the [NOTICE](NOTICE) file.

## Disclaimer
Please use this utility responsibly and comply with relevant copyright laws. The authors are not responsible for any misuse of this software.
