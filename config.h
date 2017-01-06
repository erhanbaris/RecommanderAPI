#pragma once

#define USE_GOOGLE_DENSE_HASH_MAP
#define RESERVED_SIZE 32768
#define KNEAR_SIZE 5

#define RATE_TYPE unsigned char
#define PRODUCT_TYPE unsigned long
#define USER_TYPE unsigned long
#define HTTP_SERVER_PORT 8080
#define ENABLE_CACHE1

#ifdef USE_GOOGLE_DENSE_HASH_MAP
    #include <sparsehash/dense_hash_map>
    #include <sparsehash/dense_hash_set>

    #define CUSTOM_SET google::dense_hash_set
    #define CUSTOM_MAP google::dense_hash_map

    #define INIT_MAP(map, delete_key, empty_key)  map.set_deleted_key(delete_key);  map.set_empty_key(empty_key);
    #define INT_INIT_MAP(map)  INIT_MAP(map, 0, -1)

    #define INIT_SET(map, delete_key, empty_key)  INIT_MAP(map, delete_key, empty_key);
    #define INT_INIT_SET(map)  INIT_MAP(map, 0, -1)

    #define ADD_TO_MAP(map, key, value)  map[key] = value;
#else
    #include <map>
    #include <set>

    #define CUSTOM_SET std::set
    #define CUSTOM_MAP std::map

    #define INIT_MAP(map, delete_key, empty_key)
    #define INT_INIT_MAP(map)

    #define INIT_SET(map, delete_key, empty_key)
    #define INT_INIT_SET(map)

    #define ADD_TO_MAP(map, key, value)  map[key] = value;
#endif

