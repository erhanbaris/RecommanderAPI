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


void core::splitString(const STR_TYPE &s, CHAR_TYPE delim, std::vector<STR_TYPE> &elems) {
    CUSTOM_SET<STR_TYPE> set;
    INIT_SET(set, STR(" "), STR("."));
    STRSTREAM_TYPE ss;
    ss.str(s);
    STR_TYPE item;
    while (std::getline(ss, item, delim)) {
        set.insert(item);
    }

    elems.resize(set.size());
    std::copy(set.begin(), set.end(), elems.begin());
}

std::vector<STR_TYPE> core::splitString(const STR_TYPE &s, CHAR_TYPE delim) {
    std::vector<STR_TYPE> elems;
    core::splitString(s, delim, elems);
    return elems;
}


bool core::isInteger(const STR_TYPE &s)
{
    STR_TYPE::const_iterator end = s.end();
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

void core::clearString(STR_TYPE &str) {
    STR_TYPE::iterator end = str.end();
    for (auto current = str.begin(); current != end; ++current) {

        if (((*current) >= 'A' && (*current) <= 'Z') ||
            ((*current) >= 'a' && (*current) <= 'z') ||
            ((*current) >= '0' && (*current) <= '9'))
            continue;

        (*current) = ' ';
    }
}


size_t core::realTextSize(STR_TYPE const & str) {

    size_t returnValue = 0;
    STR_TYPE::const_iterator end = str.end();
    for (auto current = str.begin(); current != end; ++current) {

        if (((*current) >= 'A' && (*current) <= 'Z') ||
            ((*current) >= 'a' && (*current) <= 'z') ||
            ((*current) >= '0' && (*current) <= '9'))
            ++returnValue;
    }

    return returnValue;
}

unsigned char * core::intToBits(size_t value)
{
	unsigned char * result = new unsigned char[sizeof(size_t)];

	for (int i = 0; i < sizeof(size_t); i++)
		result[i] = 0xFF & (value >> (i * 8));

	return result;
}

size_t core::bitsToInt(unsigned char * bits)
{
	size_t result = 0;

	//if (little_endian)
		for (int n = sizeof(size_t); n >= 0; n--)
			result = (result << 8) + bits[n];
	//else
	//	for (int n = 0; n < sizeof(IntegerType); n++)
	//		result = (result << 8) + bits[n];

	return result;
}