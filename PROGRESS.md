### fconvert - to be added
```
- image formats:
JPEG/JPG - Done
GIF - Done
PNG - Done
WebP - Done
HEIF/HEIC
TIFF - Done
BMP - Done
PSD
RAW
EPS
AI
INDD
DWG
SVG

- video formats:
MP4 (H.264) - Done
MOV - Done
AVI - Done
WMV - Done
FLV & F4V - Done
MKV - Done
WebM - Done
AVCHD
MPEG-2
3GP & 3G2 - Done
OGV (Ogg Video)
M4V- Done
ProRes
DNxHR & DNxHD

- audio formats:
(uncompressed)
WAV - Done
AIFF
PCM
DSD
(lossless compressed)
FLAC - Done
ALAC
WavPack
(lossy compressed)
MP3 - Done
AAC - *Done
OGG - Done
M4A - Done
WMA - Done
OPUS - Done

- ebook/text formats(experimental):
PDF(not recommended)
EPUB
HTML
TXT

Resolved bugs: 
* Executing with -f flag without providing file or extension, opens interactive mode - Fixed
Before: [$ fconvert -f],[$ fconvert -f file.png]{return interactive_mode();}
After:  [$ fconvert -f]{[!] Error: -f requires a file path argument [!]}
        [$ fconvert -f file.png]{File: "file.png"\n[!] Error: -f requires a file extension after file path, but found nothing. [!]}
Before: [$ fconvert -f -webp file]{Error: File '-webp' not found.}
After:  [$ fconvert -f -webp file]{[!] Error: -f requires a file path, but found a flag: -webp [!]}
Fix: 
      for(...){
        ...
        if (arg == "-f") {
            if (i + 1 >= args.size()) { // didn't find any argument
                std::cerr << "[!] Error: -f requires a file path argument [!]" << std::endl;
                return 1;
            }
            std::string next_arg = args[i + 1];
            if (next_arg[0] == '-') { // found flag instead of file path
                std::cerr << "[!] Error: -f requires a file path, but found a flag: " << next_arg << " [!]" << std::endl;
                return 1;
            }
            quick_file = args[++i];
            std::cout << quick_file << std::endl;
            continue;
        } // checks and assigns the desired format
        if (arg.size() > 1 && (arg[0] == '-' || arg[0] == '.')) {
            quick_ext = arg.substr(1);
            std::cout << quick_ext << std::endl;
            continue;
        }
    }

    if (!quick_file.empty() && quick_ext.empty()) { // if no extension is provided
        std::cerr << "[!] Error: -f requires a file extension after file path, but found nothing. [!]" << std::endl;
        return 1;
    }

    if (!quick_file.empty() && !quick_ext.empty()) { // the -f logic for quick conversion
    ....}
NOTE: [$ fconvert -webp -f file.png] works the same as [$ fconvert -f file.png -webp] for now

Added: Visible logs in Debug Mode
```
