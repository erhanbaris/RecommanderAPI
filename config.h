#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#define USE_GOOGLE_DENSE_HASH_MAP
#define RESERVED_SIZE 32768
#define KNEAR_SIZE 5
#define HTTP_SERVER_PORT 5050
#define ENABLE_CACHE

#undef ENABLE_CACHE


#define WWW 1

#if defined(_WIN32) || defined(WIN32)
	#define ucout std::wcout
	#define uendl std::endl
	#define ucin std::wcin
	#define ucerr std::wcerr
	#define STR(x) L ## x
	#define STR_TYPE std::wstring
    #define CHAR_TYPE wchar_t
	#define STRCPY wcscpy
	#define STRLEN wcslen 
	#define STRNVMP wcsncmp
	#define GET_STRING(x) core::getString(x)
	#define GET_WSTRING(x) core::getString(x)

	typedef std::wostringstream OSTRSTREAM_TYPE;
	typedef std::wofstream OFSTREAM_TYPE;
	typedef std::wostream OSTREAM_TYPE;
	typedef std::wistream ISTREAM_TYPE;
	typedef std::wifstream IFSTREAM_TYPE;
	typedef std::wistringstream ISTRSTREAM_TYPE;
	typedef std::wstringstream STRSTREAM_TYPE;
	#include <wchar.h>
#else 
	#define ucout std::cout
	#define uendl std::endl
	#define ucin std::cin
	#define ucerr std::cerr
	#define STR(x) x
	#define STR_TYPE std::string
    #define CHAR_TYPE char
	#define STRCPY strcpy
	#define STRLEN strlen
	#define STRNVMP strncmp
	#define GET_STRING(x) x
	#define GET_WSTRING(x) x

	typedef std::ostringstream OSTRSTREAM_TYPE;
	typedef std::ofstream OFSTREAM_TYPE;
	typedef std::ostream OSTREAM_TYPE;
	typedef std::istream ISTREAM_TYPE;
	typedef std::ifstream IFSTREAM_TYPE;
	typedef std::istringstream ISTRSTREAM_TYPE;
	typedef std::stringstream STRSTREAM_TYPE;
#endif


#if defined(_WIN32) || defined(WIN32)
    #define RATE_TYPE uint8_t
    #define PRODUCT_TYPE size_t
    #define USER_TYPE size_t
#else
    #define RATE_TYPE unsigned char
    #define PRODUCT_TYPE unsigned long
    #define USER_TYPE unsigned long
#endif

#ifdef USE_GOOGLE_DENSE_HASH_MAP
    #include <sparsehash/dense_hash_map>
    #include <sparsehash/dense_hash_set>

    #define CUSTOM_SET google::dense_hash_set
    #define CUSTOM_MAP google::dense_hash_map

    #define INIT_MAP(map, delete_key, empty_key)  (map).set_deleted_key(delete_key);  (map).set_empty_key(empty_key);
    #define INT_INIT_MAP(map)  INIT_MAP(map, 0, -1)

    #define INIT_SET(map, delete_key, empty_key)  INIT_MAP(map, delete_key, empty_key);
    #define INT_INIT_SET(map)  INIT_MAP(map, 0, -1)
#else
    #include <map>
    #include <set>

    #define CUSTOM_SET std::set
    #define CUSTOM_MAP std::map

    #define INIT_MAP(map, delete_key, empty_key)
    #define INT_INIT_MAP(map)

    #define INIT_SET(map, delete_key, empty_key)
    #define INT_INIT_SET(map)
#endif



#if defined(_DEBUG) || defined(DEBUG)
    #define DEBUG_WRITE(message) ucout << STR("[ DEBUG ] : ") << message << uendl;
#else
    #define DEBUG_WRITE(message)
#endif

#define LOG_WRITE(message) ucout << STR("[ LOG ] : ") << message << uendl;
#define ERROR_WRITE(message) ucout << STR("[ ERROR ] : ") << message << uendl;
