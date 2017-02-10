#pragma once

#define USE_GOOGLE_DENSE_HASH_MAP
#define RESERVED_SIZE 32768
#define KNEAR_SIZE 5
#define HTTP_SERVER_PORT 5050
#define ENABLE_CACHE

#undef ENABLE_CACHE

#define STR_TYPE utility::string_t
#define CHAR_TYPE utility::char_t
#define STR(x) _XPLATSTR(x)

#ifdef _WIN32
	#define ucout std::wcout
	#define uend std::wend
	#define ucin std::wcin
	#define ucerr std::wcerr
#else 
	#define ucout std::cout
	#define uendl std::endl
	#define ucin std::cin
	#define ucerr std::cerr
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
    #define DEBUG_WRITE(message) ucout << "[ DEBUG ] : " << message << uend;
#else
    #define DEBUG_WRITE(message)
#endif

#define LOG_WRITE(message) ucout << "[ LOG ] : " << message << uend;
#define ERROR_WRITE(message) ucout << "[ ERROR ] : " << message << uend;