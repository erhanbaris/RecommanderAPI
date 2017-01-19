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


void core::splitString(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> core::splitString(const std::string &s, char delim) {
    std::vector<std::string> elems;
    core::splitString(s, delim, elems);
    return elems;
}


bool core::isInteger(const std::string &s)
{
    string::const_iterator end = s.end();
    for (auto current = s.begin(); current != end; ++current) {

        if (((*current) < '0' || (*current) > '9'))
            return false;
    }

    return true;
}

std::locale loc;
string core::getNarrow(const std::wstring &s) {
    std::wstring::size_type length = s.length();

    std::use_facet<std::ctype<wchar_t> >(loc).narrow(s[0], '?');

    char *pc = new char[length + 1];
    std::use_facet<std::ctype<wchar_t> >(loc).narrow(s.c_str(), s.c_str() + length + 1, '?', pc);
    return string(pc);
}