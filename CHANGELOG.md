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