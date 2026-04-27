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

Bugs: (on random path without input/output folders) - Fixed
$ fconvert -f 2279192-2493561373.png -webp
terminate called after throwing an instance of 'std::filesystem::__cxx11::filesystem_error'
  what():  filesystem error: cannot create directories: Invalid argument []
Aborted                    (core dumped) fconvert -f 2279192-2493561373.png -webp
(needs full path - shouldnt if we are located in that directory)
$ fconvert -f /home/eraldo/Pictures/2279192-2493561373.png -webp
 [+] Saved: "2279192-2493561373.webp"
```
