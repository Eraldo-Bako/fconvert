#include "ebook_converter.h"
#include "path_handler.h"
#include <iostream>
#include <filesystem>
#include <string>

void ebook();
void ebook_convert_logic(fs::path in, std::string fmt, bool silent);