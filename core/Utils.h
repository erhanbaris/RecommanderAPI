#pragma once

#include "../config.h"
#include <locale>
#include <codecvt>
#include <vector>

using namespace std;

namespace core {
	static double AvgRating = 0.0;
	static size_t AvgProductCount = 0;

	static double TotalRating = 0.0;
	static size_t TotalProductCount = 0;
	static size_t TotalUserCount = 0;

	wstring getString(const string & data);
	string getString(const wstring & data);
	bool fileExists(string const &filename);
	size_t indexOf(char *text, char searchChar);

	typedef struct tIndexItem {
		PRODUCT_TYPE ProductId;
		USER_TYPE UserId;
		RATE_TYPE Rating;
	} IndexItem;


	typedef struct tUserInfo {
		tUserInfo() : TotalProduct(0), TotalRating(0.0), AvgRating(0.0), Products(vector<IndexItem*>()) {  }
		PRODUCT_TYPE Id;
		int TotalProduct;
		float TotalRating;
		float AvgRating;

		std::vector<IndexItem*> Products;
	} UserInfo;
}