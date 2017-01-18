#pragma once

#include <config.h>
#include <core/Utils.h>
#include <core/algoritm/SlopeOne.h>
#include <core/algoritm/SymSpell.h>
#include <model/Rating.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace core;
using namespace core::algoritm;

namespace core {
	namespace data {
		class BaseDataInfo {
		public:
			BaseDataInfo();

			virtual void AddUserRating(USER_TYPE customerId, PRODUCT_TYPE productId, RATE_TYPE ratingChar) = 0;
			virtual void AddProduct(PRODUCT_TYPE movieId, wstring title) = 0;
			virtual void Prepare() = 0;

			algoritm::SlopeOne slopeOne;
			SymSpell symspell;

			CUSTOM_MAP<USER_TYPE, vector<model::Rating>> userRatings;

			CUSTOM_MAP<PRODUCT_TYPE, std::vector<IndexItem*> > productMap;
			CUSTOM_MAP<USER_TYPE, UserInfo> userMap;

			CUSTOM_MAP<PRODUCT_TYPE, wstring> productInfos;
			CUSTOM_MAP<USER_TYPE, vector<pair<PRODUCT_TYPE, wstring>>> recommendCacheForUser;
		};
	}
}
