#include "types.h"

using namespace std;

wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
wstring core::getString(const string &data) {
    return converter.from_bytes(data);
}

string core::getString(const wstring &data) {
    return converter.to_bytes(data);
}
