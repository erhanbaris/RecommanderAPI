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
#include "../model/Rating.h"
#include "../types.h"

#define CALCULATE_METHOD() if (currentUser != 0) \
{ \
	float denominator = sqrtf(sum_x2 - powf(sum_x, 2) / n) * sqrtf(sum_y2 - (powf(sum_y, 2)) / n); \
\
	if (!(abs(denominator) < 0.000001)) \
	{ \
		float distance = (sum_xy - ((sum_x * sum_y) / n)) / denominator; \
		returnValue.push_back(pair<size_t, float>(currentUser, distance)); \
	} \
}

using namespace std;
using namespace model;

namespace core {
	namespace algoritm {

		class PearsonCorrelationCoefficien
		{
		public:
			vector<pair<PRODUCT_TYPE, float>> Recommend(const CUSTOM_MAP<PRODUCT_TYPE, vector<Rating>> * users, const USER_TYPE & userId)
			{
				vector<pair<PRODUCT_TYPE, float>> returnValue;

				auto user = users->find(userId);
				if (user == users->end())
					return returnValue;
				returnValue.reserve(128);

				CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> userProducts;
				INT_INIT_MAP(userProducts);

				auto userEnd = user->second.end();
				for (auto it = user->second.begin(); it != userEnd; ++it)
					userProducts[it->ProductId] = it->Rate;

				size_t counter1 = 0;
				vector<pair<PRODUCT_TYPE, IndexItem*>> items;
				items.reserve(1024);

				for (auto it = user->second.cbegin(); it != userEnd; ++it)
				{
					auto indexResult2 = (*this->productIndex)[it->ProductId];

					auto indexResult2End = indexResult2.end();
					for (auto it2 = indexResult2.begin(); it2 != indexResult2End; ++it2)
					{
						IndexItem* indexItem = (*it2);

						auto& userInfo = this->userIndex->find(indexItem->UserId)->second;

						if (indexItem->UserId == userId || userInfo.TotalProduct <= core::AvgProductCount || userInfo.AvgRating <= core::AvgRating)
							continue;

						items.push_back(pair<PRODUCT_TYPE, IndexItem*>(indexItem->UserId, *it2));
						++counter1;
					}
				}

				std::sort(items.begin(), items.end());

				std::cout << "For Counter : " << counter1 << std::endl;
				returnValue = CompareUser(userProducts, items);
				std::cout << "Total Item Finded : " << returnValue.size() << std::endl;

				std::sort(returnValue.begin(), returnValue.end(), [](pair<PRODUCT_TYPE, float> const & left, pair<PRODUCT_TYPE, float> const & right) {
					return left.second > right.second;
				});

				return returnValue;
			}

			vector<pair<PRODUCT_TYPE, float>> CompareUser(CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> & user1, vector<pair<PRODUCT_TYPE, IndexItem*>> & comparableUsers) const
			{
				float sum_xy = 0;
				float sum_x = 0;
				float sum_y = 0;
				float sum_x2 = 0;
				float sum_y2 = 0;
				size_t n = 0;

				USER_TYPE currentUser = 0;

				vector<pair<PRODUCT_TYPE, float>> returnValue;
				auto comparableUsersEnd = comparableUsers.end();
				for (auto it = comparableUsers.begin(); it != comparableUsersEnd; ++it)
				{
					IndexItem* item = it->second;

					if (currentUser != item->UserId)
					{
						sum_xy = 0;
						sum_x = 0;
						sum_y = 0;
						sum_x2 = 0;
						sum_y2 = 0;
						n = 0;

						CALCULATE_METHOD();

						currentUser = item->UserId;
					}

					++n;
					RATE_TYPE x = user1[item->ProductId];
					RATE_TYPE y = item->Rating;

					sum_xy += x * y;
					sum_x += x;
					sum_y += y;
					sum_x2 += powf(x, 2);
					sum_y2 += powf(y, 2);
				}

				CALCULATE_METHOD();

				return returnValue;
			}

			void SetProductIndex(CUSTOM_MAP<PRODUCT_TYPE, vector<IndexItem*>>* pProductIndex)
			{
				this->productIndex = pProductIndex;
			}

			void SetUserIndex(CUSTOM_MAP<USER_TYPE, UserInfo>* pUserIndex)
			{
				this->userIndex = pUserIndex;
			}

		private:
			CUSTOM_MAP<PRODUCT_TYPE, vector<IndexItem*>>* productIndex;
			CUSTOM_MAP<USER_TYPE, UserInfo>* userIndex;
		};
	}
}