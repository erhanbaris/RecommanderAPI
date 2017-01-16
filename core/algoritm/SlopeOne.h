#pragma once

#include <vector>
#include <config.h>
#include <core/Utils.h>

using namespace std;

namespace core {
	namespace algoritm {
		class SlopeOne {
		public:
			SlopeOne();

			void AddUsers(CUSTOM_MAP<USER_TYPE, core::UserInfo> & users);
			void AddUserRatings(core::UserInfo & userRatings);
			vector<pair<PRODUCT_TYPE, float>> * Predict(CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> & userRatings);
		};
	}
}
