#pragma once

#include "../../config.h"
#include "../types.h"
#include "../algoritm/SlopeOne.h"
#include "../../model/Rating.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace core;
using namespace core::algoritm;

namespace core {
	namespace data {
		class BaseDataInfo {
		public:
			BaseDataInfo()
			{
				INT_INIT_MAP(productInfos);
				INT_INIT_MAP(userRatings);
				INT_INIT_MAP(recommendCacheForUser);

				INT_INIT_MAP(productMap);
				INT_INIT_MAP(userMap);
			}

			virtual void AddUserRating(USER_TYPE customerId, PRODUCT_TYPE productId, RATE_TYPE ratingChar) = 0;
			virtual void AddProduct(PRODUCT_TYPE movieId, wstring title) = 0;
			virtual void Prepare() = 0;

			algoritm::SlopeOne slopeOne;

			CUSTOM_MAP<size_t, vector<model::Rating>> userRatings;

			CUSTOM_MAP<PRODUCT_TYPE, std::vector<IndexItem*> > productMap;
			CUSTOM_MAP<USER_TYPE, UserInfo> userMap;

			CUSTOM_MAP<PRODUCT_TYPE, wstring> productInfos;
			CUSTOM_MAP<USER_TYPE, vector<pair<PRODUCT_TYPE, wstring>>> recommendCacheForUser;
		};

		class DataInfo : public BaseDataInfo
		{
		public:
			DataInfo()
			{
				productMapEnd = productMap.end();
				userMapEnd = userMap.end();
			}

			void AddUserRating(USER_TYPE customerId, PRODUCT_TYPE productId, RATE_TYPE rating) override
			{
				try {
					IndexItem* indexItem = new IndexItem();
					indexItem->ProductId = productId;
					indexItem->UserId = customerId;
					indexItem->Rating = rating;

					if (productMap.find(productId) == productMapEnd)
					{
						this->productMap[productId] = std::vector<IndexItem*>();
						this->productMapEnd = this->productMap.end();
					}

					if (userMap.find(customerId) == userMapEnd)
					{
						this->userMap[customerId] = UserInfo();
						this->userMapEnd = userMap.end();

						++core::TotalUserCount;
					}

					auto & userMapCache = userMap[customerId];
					productMap[productId].push_back(indexItem);
					userMapCache.Products.push_back(indexItem);

					if (userRatings.find(customerId) == userRatings.end())
						userRatings[customerId] = vector<model::Rating>();

					userMapCache.TotalRating += rating;
					++userMapCache.TotalProduct;
					userMapCache.AvgRating += userMapCache.TotalRating / userMapCache.TotalProduct;

					core::TotalProductCount++;
					core::TotalRating += rating;

					userRatings[customerId].push_back(model::Rating(productId, rating));
				}
				catch (const std::exception& e)
				{
					cout << "Rating : " << e.what() << endl;
				}
			}

			void AddProduct(PRODUCT_TYPE movieId, wstring title) override
			{
				productInfos[movieId] = title;
			};

			void Prepare() override
			{
				core::AvgProductCount = (core::TotalProductCount / core::TotalUserCount);
				core::AvgRating = core::TotalRating / core::TotalProductCount;

				cout << "Slope One Started >>" << endl;
				slopeOne.AddUsers(userMap);
				cout << "<< Slope One Finished" << endl;
			};

		protected:
			CUSTOM_MAP<PRODUCT_TYPE, vector<IndexItem*>>::iterator productMapEnd;
			CUSTOM_MAP<USER_TYPE, UserInfo>::iterator  userMapEnd;
		};



		template<class DataInfo, class DataSource>
		class RecommandExecuter
		{
		public:
			virtual void SetDataInfo(shared_ptr<DataInfo> pInfo)
			{
				this->info = pInfo;
			}

			virtual void SetDataSource(shared_ptr<DataSource> pSource)
			{
				this->source = pSource;
			}

			shared_ptr<DataInfo> GetDataInfo(shared_ptr<DataInfo> pInfo)
			{
				return this->info;
			}

			shared_ptr<DataSource> GetDataSource()
			{
				return this->source;
			}

		private:
			shared_ptr<DataInfo> info;
			shared_ptr<DataSource> source;
		};
	}
}
