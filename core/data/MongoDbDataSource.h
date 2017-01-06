#pragma once

#ifdef MONGODB

#include <mongocxx/client.hpp>
#include <mongocxx/pipeline.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>


#include <sparsehash/dense_hash_map>

#include <vector> 
#include <stdio.h>
#include <string>

#include "Rating.h"

#include "types.h"
#include "BaseDataSource.h"
#include "BaseDataInfo.h"

using namespace std;
using namespace core;

namespace core
{
	namespace data {
		template<class T = DataInfo>
		class MongoDbDataSource : public BaseDataSource<T> {
		public:
			explicit MongoDbDataSource(string pUrl) : url(pUrl)
			{
				mongocxx::instance::current();
				mongocxx::uri uris(url);
				mongoClient = new mongocxx::client(uris);
				database = mongoClient->database("ml");
			}

			void LoadData() override
			{
				bsoncxx::builder::stream::document filter{};
				filter << "Rating" << bsoncxx::builder::stream::open_document << "$gt" << 0 << bsoncxx::builder::stream::close_document;

				auto productReviewCursor = database.collection("product_review").find(filter.view());
				for (auto doc : productReviewCursor)
				{
					string oid = doc["ProductId"].get_oid().value.to_string();
					size_t customerId = stoi(doc["CustomerId"].get_utf8().value.to_string());
					size_t productId = hash<string>{}(oid);
					size_t rating = doc["Rating"].get_int32().value;

					this->data->AddUserRating(customerId, productId, rating * 10);
				}

				auto productCursor = database.collection("product").find({});
				for (auto doc : productCursor)
				{
					size_t productId = hash<string>{}(doc["_id"].get_oid().value.to_string());
					string name = doc["Name"].get_utf8().value.to_string();

					this->data->AddProduct(productId, getString(name));
				}

				data->Prepare();
			}

			~MongoDbDataSource() override
			{
				delete mongoClient;
			}
			

		private:
			string url;

			mongocxx::v_noabi::database database;
			mongocxx::client* mongoClient;
		};
	}
}

#endif