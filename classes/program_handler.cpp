// fconvert v2.3.0 | Copyright (c) 2023-2026 Eraldo Bako
// Licensed under the Apache License, Version 2.0 (the "License")
// Maintainer: eraldobako@gmail.com

#include "program_handler.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>

// wanted to use something like cout.rdbuf(), but manually writing to the log file felt more practical and easy -.-
void Program::log(const std::string& msg, bool print, bool write) {
    std::string time = Program::Get::currentTimestamp();
    std::string liveLog = "[" + time + "]";

    if (Program::debug_mode) {
        if (print) std::clog << "\n[DEBUG] ------ " << msg << "\n" << std::flush;
        if (write) liveLog.append(" - [DEBUG] ------ " + msg + "\n");
    } else liveLog.append(" ------ " + msg + "\n");
    
    std::ofstream logFile(Program::activeLogPath, std::ios::app);
    if (logFile.is_open()) {
        logFile << liveLog;
        logFile.close();
    } else {
        std::cerr << "[!] Error: Could not open log file! [!]\n";
    }
}

// aggressive, using this as a last resort if sth goes rly bad
void Program::end(const std::string& msg) {
    std::cerr << msg << std::endl;
    std::cerr << "[!!] Critical Warning: Forcefully ending the program. [!!]" << std::endl;

    try {
        Program::log(msg);
    } catch (const std::exception& e) {
        std::cerr << "[!!] Failed to write to log file: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[!!] Failed to write to log file due to an unknown error." << std::endl;
    }
    
    std::fflush(nullptr);
    std::exit(EXIT_FAILURE);
}

void Program::print(const std::string& msg, bool error) {
    if (!error) {
        std::cout << msg << std::flush;

        Program::log("[fconvert] - " + msg, false);
    } else {
        std::cerr << msg << std::flush;
        
        Program::log("[fconvert Error] - " + msg, false);
    }
}

// variable_input = Program::Get::input("What is your variable? ", false, true);