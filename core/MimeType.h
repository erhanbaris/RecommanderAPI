#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iostream>
#include <config.h>

using namespace std;

namespace core {
    class MimeType {
    public:
        MimeType();
		string GetMimeType(string const &extension);
        void AddMimeType(string const &extension, string const &mime);

    private:
        CUSTOM_MAP<string, string> mimeTypes;
        CUSTOM_MAP<string, string>::iterator endMimeType;

        void initMimeTypes();
    };
}