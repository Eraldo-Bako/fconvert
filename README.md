<div id="fconvert-logo" align="center">
    <br />
    <img src="./branding/fconvert.svg" alt="fconvert Logo" width="200"/>
    <h1>fconvert</h1>
    <h3>A fast CLI converter for Images, Videos, Audios, and Ebooks</h3>
</div>

**fconvert** is a fast, intentional, and hardware-agnostic C++ command-line utility. Designed for Arch Linux and other Unix-like systems, it provides a seamless way to convert images, videos, and audio files using a smart "Sandbox" directory logic.

---

## --> Key Features

* **Smart Routing:** Automatically detects intent. Want audio from a video? Just run `fconvert -f movie.mp4 -mp3`.
* **Sandbox Workflow:** Automatically monitors `input_file/` and outputs to `output_file/` to keep your system clean.
* **Conflict Resolution:** Handles existing files gracefully (auto-renaming or prompt-to-overwrite).
* **CLI & Interactive:** Use rapid flags for automation or the guided menu for ease of use.
* **Hardware Agnostic:** Optimized for high-quality CPU encoding, ensuring portability across different environments.

---

## --> Supported Formats

| Category | Extensions |
| :--- | :--- |
| **Images** | `jpg`, `jpeg`, `png`, `webp`, `tiff`, `bmp` |
| **Audio** | `mp3`, `wav`, `flac`, `ogg`, `aac`, `m4a`, `opus`, `wma` |
| **Video** | `mp4`, `mkv`, `mov`, `webm`, `avi`, `m4v`, `flv`, `3gp` |
| **eBook** | `pdf`, `epub`, `html`, `txt`, `docx` |

> **Note:** Inputting a video file with an audio target extension (e.g., `.mp4` to `-mp3`) will automatically trigger a high-quality audio extraction.

---

## --> Installation

### Prerequisites
Ensure you have the following installed on your system:
* **C++17** compatible compiler (GCC/Clang)
* **FFmpeg** (for video and audio processing)
* **OpenCV 4** (for image processing)
* **Pandoc** (for experimental document processing)

### Build Instructions
For Arch Linux and most Linux distributions, use the following command:

```bash
cd fconvert
g++ -o fconvert main.cpp classes/*.cpp `pkg-config --cflags --libs opencv4` -lstdc++fs
```
or build it with CMake if configured in your system
```bash
cd fconvert
cmake -B build -S
cmake --build build
cp build/fconvert ./fconvert # Altrnatively if you want the binary to be located in the
                             # ~/fconvert directory, instead of the ~/fconvert/build
```
### Installation (Arch Linux)
Clone the repo and use `makepkg` to install system-wide:
```bash
git clone https://github.com/Eraldo-Bako/fconvert
cd fconvert
makepkg -si
```
> **Note:** You could use a package helper like ```yay``` or ```paru``` since the package is in the AUR: ```yay -S fconvert``` or ```paru -S fconvert```
## --> Usage
> **Note:** If you installed the package via `makepkg`, you can simply use `fconvert`. If you are running the binary locally from the build folder, use `./fconvert`.
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
```bash
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

## --> Flags
* ```-v```, ```--version``` : Show package version and licensing information.
* ```-h```, ```--help``` : Show the help message and supported formats.

* ```-d```, ```--debug```: Enable debug mode for detailed path-resolution logging.

* ```-f <file> -<ext>```: Execute a quick conversion.
* ```-f <file> .<ext>```: Alternatively you can use `.<ext>` is a valid usage.
## --> Contributing
Contributions are welcome! If you find a bug or have a feature request, please open an issue or submit a pull request.
## --> License
* Code: [MIT](LICENSE) 
* Branding/Logos: [CC BY-NC-ND 4.0](branding/LICENSE)
## --> Disclaimer
Please use this utility responsibly and comply with relevant copyright laws. The authors are not responsible for any misuse of this software.