#pragma once

#include <vector>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <functional>
#include <locale>

#include "../../config.h"
#include "../../model/Rating.h"
#include "../Utils.h"

using namespace std;
using namespace model;

namespace core {
	namespace algoritm {

		class PearsonCorrelationCoefficien
		{
		public:
			vector<pair<PRODUCT_TYPE, float>> Recommend(const CUSTOM_MAP<PRODUCT_TYPE, vector<Rating>> * users, const USER_TYPE & userId);
			vector<pair<PRODUCT_TYPE, float>> CompareUser(CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> & user1, vector<pair<PRODUCT_TYPE, IndexItem*>> & comparableUsers) const;

			void SetProductIndex(CUSTOM_MAP<PRODUCT_TYPE, vector<IndexItem*>>* pProductIndex);
			void SetUserIndex(CUSTOM_MAP<USER_TYPE, UserInfo>* pUserIndex);

		private:
			CUSTOM_MAP<PRODUCT_TYPE, vector<IndexItem*>>* productIndex;
			CUSTOM_MAP<USER_TYPE, UserInfo>* userIndex;
		};
	}
}