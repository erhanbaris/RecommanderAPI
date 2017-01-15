#include <core/Utils.h>

using namespace std;

wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
wstring core::getString(const string &data) {
    return converter.from_bytes(data);
}

string core::getString(const wstring &data) {
    return converter.to_bytes(data);
}


bool core::fileExists(string const &filename) {
    if (FILE *file = fopen(filename.c_str(), "r")) {
        fclose(file);
        return true;
    }

    return false;
}

size_t core::indexOf(char *text, char searchChar) {
    size_t totalLength = strlen(text);
    char *e = text + totalLength;
    size_t idx = 0;
    while (*e-- != searchChar && totalLength != idx)
        idx++;

    return totalLength - idx;
}