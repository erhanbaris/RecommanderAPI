#include <core/Utils.h>

using namespace std;
using boost::locale::conv::utf_to_utf;

wstring core::getString(const string &data) {
    return utf_to_utf<wchar_t>(data.c_str(), data.c_str() + data.size());
}

string core::getString(const wstring &data) {
    return utf_to_utf<char>(data.c_str(), data.c_str() + data.size());
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