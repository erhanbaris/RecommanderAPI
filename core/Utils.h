#pragma once

#include <config.h>
#include <locale>
#include <string>
#include <sstream>
#include <vector>
#include <boost/locale/encoding_utf.hpp>
#include <chrono>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::duration<float> fsec;

using namespace std;

#define isInteger(s, r) \
{\
    r = true;\
	STR_TYPE::const_iterator end = s.end();\
	for (auto current = s.begin(); current != end; ++current) {\
	if (((*current) < '0' || (*current) > '9'))\
	{ r = false; break; }\
}\
}\

#define clearString(str) { \
    STR_TYPE::iterator end = str.end();\
    for (auto current = str.begin(); current != end; ++current) {\
        if (((*current) >= 'A' && (*current) <= 'Z') ||\
            ((*current) >= 'a' && (*current) <= 'z') ||\
            ((*current) >= '0' && (*current) <= '9'))\
            continue;\
        (*current) = ' ';\
    }\
}

namespace core {
	static double AvgRating = 0.0;
	static size_t AvgProductCount = 0;

	static double TotalRating = 0.0;
	static size_t TotalProductCount = 0;
	static size_t TotalUserCount = 0;

	wstring getString(const string &data);

	string getString(const wstring &data);

	bool fileExists(string const &filename);

	void splitString(const STR_TYPE &s, CHAR_TYPE delim, std::vector<STR_TYPE> &elems);

	std::vector<STR_TYPE> splitString(const STR_TYPE &s, CHAR_TYPE delim);

	string getNarrow(const std::wstring &s);

	size_t realTextSize(STR_TYPE const &str);

	template<typename T>
	void intToBytes(T f, char *& result, int & size);

	template<typename T>
	T bytesToInt(char * result, int size);

	unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );

	typedef struct tIndexItem {
		PRODUCT_TYPE ProductId;
		USER_TYPE UserId;
		RATE_TYPE Rating;
	} IndexItem;


	typedef struct tUserInfo {
		tUserInfo() : TotalProduct(0), TotalRating(0.0), AvgRating(0.0), Products(vector<IndexItem *>()) {}

		PRODUCT_TYPE Id;
		int TotalProduct;
		float TotalRating;
		float AvgRating;

		std::vector<IndexItem *> Products;
	} UserInfo;
}
