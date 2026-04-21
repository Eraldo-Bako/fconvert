# fconvert 🛠️

**fconvert** is a fast, intentional, and hardware-agnostic C++ command-line utility. Designed for Arch Linux and other Unix-like systems, it provides a seamless way to convert images, videos, and audio files using a smart "Sandbox" directory logic.

---

## ✨ Key Features

* **Smart Routing:** Automatically detects intent. Want audio from a video? Just run `fconvert -f movie.mp4 -mp3`.
* **Sandbox Workflow:** Automatically monitors `input_file/` and outputs to `output_file/` to keep your system clean.
* **Conflict Resolution:** Handles existing files gracefully (auto-renaming or prompt-to-overwrite).
* **CLI & Interactive:** Use rapid flags for automation or the guided menu for ease of use.
* **Hardware Agnostic:** Optimized for high-quality CPU encoding, ensuring portability across different environments.

---

## 📊 Supported Formats

| Category | Extensions |
| :--- | :--- |
| **Images** | `jpg`, `jpeg`, `png`, `webp`, `tiff`, `bmp` |
| **Audio** | `mp3`, `wav`, `flac`, `ogg`, `m4a`, `opus` |
| **Video** | `mp4`, `mkv`, `mov`, `webm`, `avi`, `flv` |

> **Note:** Inputting a video file with an audio target extension (e.g., `.mp4` to `-mp3`) will automatically trigger a high-quality audio extraction.

---

## 🚀 Installation

### Prerequisites
Ensure you have the following installed on your system:
* **C++17** compatible compiler (GCC/Clang)
* **FFmpeg** (for video and audio processing)
* **OpenCV 4** (for image processing)

### Build Instructions
For Arch Linux and most Linux distributions, use the following command:

```bash
g++ -o fconvert main.cpp classes/*.cpp `pkg-config --cflags --libs opencv4` -lstdc++fs
```
### Installation (Arch Linux)
Clone the repo and use `makepkg` to install system-wide:
```bash
git clone https://github.com/Eraldo-Bako/fconvert
cd fconvert
makepkg -si
```
> **Note:** You could use a package helper like ```yay``` or ```paru``` once the package is in the AUR.
## 📖 Usage
> **Note:** If you installed the package via `makepkg`, you can simply use `fconvert`. If you are running the binary locally from the build folder, use `./fconvert`.
### Quick Convert (CLI Mode)
Convert a file instantly without entering the menu:

```bash
fconvert -f image.png -webp
fconvert -f music.wav -flac
fconvert -f video.mp4 -mp3  # Automatically extracts high-quality audio
```
### Interactive Mode
Simply run the executable to enter the guided interface:
```bash
fconvert
```
### Sandbox Logic
1. Place your files in the input_file/ folder.
2. Run fconvert.
3. Find your processed files in the output_file/ folder.

## ⌨️ Flags
* ```-h```, ```--help``` : Show the help message and supported formats.

* ```-d```, ```--debug```: Enable debug mode for detailed path-resolution logging.

* ```-f <file> -<ext>```: Execute a quick conversion.
## 🤝 Contributing
Contributions are welcome! If you find a bug or have a feature request (like hardware-accelerated encoding profiles), please open an issue or submit a pull request.
## 📄 License
This project is licensed under the MIT License.
## ⚖️ Disclaimer
Please use this utility responsibly and comply with relevant copyright laws. The authors are not responsible for any misuse of this software.