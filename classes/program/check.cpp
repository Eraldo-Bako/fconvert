// fconvert v2.4.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "check.hpp"

#include <cstdlib>
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
#endif

bool Program::Check::is_running_as_root() {
#ifdef _WIN32
    BOOL is_admin = FALSE;
    PSID administrators_group = NULL;

    SID_IDENTIFIER_AUTHORITY nt_authority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&nt_authority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                                 &administrators_group)) {
        CheckTokenMembership(NULL, administrators_group, &is_admin);
        FreeSid(administrators_group);
                                 }
    return is_admin == TRUE;
#else
    return (geteuid() == 0); // 0 means root
#endif
}

bool Program::Check::command_exists(const std::string& win_cmd, const std::string& nix_cmd) {
#ifdef _WIN32
    return std::system((win_cmd + " >nul 2>nul").c_str()) == 0;
#else
    return std::system((nix_cmd + " >/dev/null 2>&1").c_str()) == 0;
#endif
}

bool Program::Check::ffmpeg() {
    return Program::Check::command_exists("where ffmpeg", "command -v ffmpeg");
}

bool Program::Check::vtracer() {
    return Program::Check::command_exists("where vtracer", "command -v vtracer");
}

bool Program::Check::imghost() {
    return Program::Check::command_exists("where magick", "command -v magick") && 
           Program::Check::command_exists("where gswin64c", "command -v gs");
}

bool Program::Check::libraw() {
#ifdef _WIN32
    return true; 
#else
    return Program::Check::command_exists("", "pkg-config --exists libraw");
#endif
}

bool Program::Check::opencv() {
#if defined(_WIN32) || defined(__APPLE__)
    return true; 
#else
    return Program::Check::command_exists("", "pkg-config --exists opencv4 || pkg-config --exists opencv5"); // opencv5 for archlinux
#endif
}

bool Program::Check::pandoc() {
    return Program::Check::command_exists("where pandoc", "command -v pandoc");
}

bool Program::Check::pdfEngine() {
    if (Program::Check::command_exists("where typst", "command -v typst")) return true;
    if (Program::Check::command_exists("where weasyprint", "command -v weasyprint")) return true;
    if (Program::Check::command_exists("where xelatex", "command -v xelatex")) return true;
    return false;
}