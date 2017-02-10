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
        STR_TYPE GetMimeType(STR_TYPE const &extension);
        void AddMimeType(STR_TYPE const &extension, STR_TYPE const &mime);

    private:
        CUSTOM_MAP<STR_TYPE, STR_TYPE> mimeTypes;
        CUSTOM_MAP<STR_TYPE, STR_TYPE>::iterator endMimeType;

        void initMimeTypes();
    };
}