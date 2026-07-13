### fconvert - to be added
```
<= : program can convert to that format
=> : format can be read to be converted into another format
<=> : - program can convert to that format
      - format can be read to be converted into another format
- image formats:
JPEG/JPG - Done (<=>)
GIF - Done (<=>)
PNG - Done (<=>)
WebP - Done (<=>)
HEIF/HEIC - Done (<=>)
TIFF/TIF - Done (<=>)
BMP - Done (<=>)
PSD - Done (<=>)
PDF - Done (<=>)
XCF - Done (<=>)
ICO - Done (<=>)
RAW(.cr2, .nef, .arw, .dng, .crw) - Done (=>)
EPS - Done (<=>)
AI - Done (<=>)
SVG - Done (<=>)

- video formats:
MP4 (H.264) - Done (<=>)
MOV - Done (<=>)
AVI - Done (<=>)
WMV - Done (<=>)
FLV & F4V - Done (<=>)
MKV - Done (<=>)
WebM - Done (<=>)
AVCHD - Done (<=>)
MPEG-2 - Done (<=>)
3GP & 3G2 - Done (<=>)
OGV (Ogg Video) - Done (<=>)
M4V- Done (<=>)
ProRes - Done (<=>)
DNxHR & DNxHD - Done (<=>)

- audio formats:
(uncompressed)
WAV - Done (<=>)
AIFF - Done (<=>)
PCM - Done (<=>)
DSD - Done (<=>)
(lossless compressed)
FLAC - Done (<=>)
ALAC - Done (<=>)
WavPack - Done (<=>)
(lossy compressed)
MP3 - Done (<=>)
AAC - *Done (<=>)
OGG - Done (<=>)
M4A - Done (<=>)
WMA - Done (<=>)
OPUS - Done (<=>)

- ebook/text formats(experimental):
PDF(not recommended)
EPUB
HTML
TXT

- archive formats(maybe next or next-next version or not):
ZIP
RAR
TAR
7z
```
### fconvert - classes/functions
```cpp
Classes: 
program_handler:
   Classes:
      - Program::debug_mode - The variable that controls if DEBUG MODE is active or not
      - Program::activeLogPath - The variable that holds the path of the log file
      - Program::end(const std::string& msg) - The function that forcefully ends the program
      - Program::log(const std::string& msg, bool print = true, bool write = true) - The logging function
      - Program::print(const std::string& msg, bool error = false) - The printing function
      - Program::clearCache() - Clears the cache in the log directory
   Subclasses:
      - Namespace Build:
            Program::Build::command(const std::string& type, const std::string& input, const std::string& output, const std::string& params) - The function that constructs the FFmpeg/Pandoc commands
      
      - Namespace Check:
            Program::Check::is_running_as_root() - returns true if it detects that it is running as administrator/root
            Program::Check::command_exists(const std::string& win_cmd, const std::string& nix_cmd) - Executes the existence of the praised command
            Program::Check::ffmpeg() - Praises the method and name to find FFmpeg
            Program::Check::imghost() - Praises the method and name to find ImageMagick and GhostScript
            Program::Check::libraw() - Praises the method and name to find LibRaw
            Program::Check::opencv() - Praises the method and name to find OpenCV
            Program::Check::pandoc() - Praises the method and name to find Pandoc
            Program::Check::pdfEngine() - Praises the method and name to find one of the supported PDF Engines

      - Namespace 
            Program::Color {

                  inline constexpr std::string_view RESET = "\033[0m";
                  inline constexpr std::string_view CYAN = "\033[36m";
                  inline constexpr std::string_view GREEN = "\033[32m";
                  inline constexpr std::string_view YELLOW = "\033[33m";
                  inline constexpr std::string_view RED = "\033[1;31m";

            } - Colors the text that appears on screen. Just for connvinience, instead of using ANSII escape sequences manually

      - Namespace Get:
            Program::Get::logDirectory() - Determines the log directory:
                  Windows: 
                        - %APPDATA%\fconvert\Logs
                        - working directory if the first method fails
                  Unix/-like: 
                        - ${XDG_STATE_HOME}/fconvert
                              - Default: ~/.local/state (/home/your-username/.local/state)
                              - Check with: echo "${XDG_STATE_HOME:-$HOME/.local/state}"
                        - If your custom ${XDG_STATE_HOME} path fails, it manually writes to:
                              ~/.local/state/fconvert
                        - If that fails to then the logs will be in the working directory
                  - might change in the future
            Program::Get::currentTimestamp() - Gets the current date and time and returns it as a readable string
            Program::Get::input(const std::string& prompt, bool lower = false, bool useWS = true) - Custom input function
      
      - Namespace Make:
            Program::Make::logFile(const std::string& time) - Creates the logfile and returns the location
            //REPLACED: Program::Make::tempFile(const std::filesystem::path& original_inputPath) - Makes a temporary file, unused - will be used in the next update(probably)
secure_conversion_session:
   public:
      - SecureConversionSession::SecureConversionSession(const std::filesystem::path& originalInput, const std::string& targetExtension) - The constructor that safely stages the original file into an isolated environment
      - SecureConversionSession::~SecureConversionSession() - The destructor that will automatically clean up whatever the constructor made in the cache dir
      - commit(const std::filesystem::path& finalUserDestination) - Moves the output back to the specified/target output directory
```
### fconvert - Warnings
```
The first thing that happens when fconvert runs is check if it running with elevated privileges(we dont want fconvert -cc to run with such privileges).
The Warning Message:

=================================================================
[CRITICAL WARNING] fconvert is running with elevated privileges.
=================================================================

Executing file conversions as root bypasses vital OS safeguards.
Malformed files or unexpected behavior CAN irreversibly corrupt
system binaries or compromise OS integrity.

=================================================================

If you do not know what you are doing or what is happening,
abort this operation immediately to prevent startup!

=================================================================
Type 'YES, I ACCEPT THE RISK - PROCEED' to proceed: <user-input-here>
Type 'PROCEED' to proceed: <i-wonder-what-this-is-referencing>
```