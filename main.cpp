#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iostream>

#pragma warning(disable:4503)

#include "config.h"
#include "core/data/BaseDataSource.h"
#include "core/data/CvsDataSource.h"
//#include "MongoDbDataSource.h"
#include "core/algoritm/SlopeOne.h"
#include <vector>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <functional>
#include <locale>
#include <sstream>

#include "crow.h"

#include "core/algoritm/PearsonCorrelationCoefficien.h"
#include "model/Rating.h"

#include "core/types.h"
#include "core/data/BaseDataInfo.h"


#include <map>


#pragma execution_character_set("utf-8")

using namespace std;
using namespace std::chrono;
using namespace model;
using namespace core::algoritm;



core::data::BaseDataSource<core::data::DataInfo>* dataSource;
PearsonCorrelationCoefficien mDistance;

std::string escapeJsonString(const std::string& input) {
    std::ostringstream ss;
    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
        //C++98/03:
        //for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            case '/': ss << "\\/"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << *iter; break;
        }
    }
    return ss.str();
}

vector<pair<PRODUCT_TYPE, wstring>> recommend(USER_TYPE userId)
{
    vector<pair<PRODUCT_TYPE, wstring>> recommendedProducts;
    CUSTOM_MAP<PRODUCT_TYPE, float> recommendedProductIds;
    INT_INIT_MAP(recommendedProductIds);

    CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> userRatings;
    INT_INIT_MAP(userRatings);

    auto & userDatas = dataSource->Data()->userMap[userId].Products;
    auto endData = userDatas.end();
    for (auto it = userDatas.begin(); it != endData; ++it)
        userRatings[(*it)->ProductId] = (*it)->Rating;

    const auto& results = dataSource->Data()->slopeOne.Predict(userRatings);
    //const auto& results = mDistance.Recommend(&dataSource->Data()->userRatings, userId);
    if (results.size() > 0)
    {
        auto nearestUser = results.at(0);

        auto nearestUserRatings = dataSource->Data()->userRatings[nearestUser.first];
        auto currentUserRatings = dataSource->Data()->userRatings[userId];
        auto currentUserRatingsEnd = currentUserRatings.end();

        float totalDistance = 0.0;
        size_t counter = 0;
        auto resultsEnd = results.cend();
        for (auto it = results.cbegin(); it != resultsEnd && counter < KNEAR_SIZE; ++it)
        {
            totalDistance += it->second;
            ++counter;
        }

        counter = 0;
        for (auto nearest = results.cbegin(); nearest != resultsEnd && counter < KNEAR_SIZE; ++nearest)
        {
            float weight = nearest->second / totalDistance;
            auto neighborRatings = dataSource->Data()->userRatings[nearest->first];

            auto neighborRatingsEnd = neighborRatings.cend();
            for (auto artist = neighborRatings.cbegin(); artist != neighborRatingsEnd; ++artist)
            {
                const auto& artistItem = (*artist);

                bool hasProduct = false;
                for (auto it = currentUserRatings.begin(); it != currentUserRatingsEnd; ++it)
                {
                    if (it->ProductId == artistItem.ProductId)
                    {
                        hasProduct = true;
                        break;
                    }
                }

                if (hasProduct)
                {
                    auto recommendItem = recommendedProductIds.find(artistItem.ProductId);
                    if (recommendItem == recommendedProductIds.end())
                        recommendedProductIds[artistItem.ProductId] = static_cast<float>(artistItem.Rate) * weight;
                    else
                        recommendItem->second = recommendItem->second + (static_cast<float>(artistItem.Rate) * weight);
                }
            }

            ++counter;
        }
    }


    vector<pair<PRODUCT_TYPE, float>> sortIds;
    auto recommendedProductIdsEnd = recommendedProductIds.end();
    for (auto it = recommendedProductIds.begin(); it != recommendedProductIdsEnd; ++it)
        sortIds.push_back(pair<PRODUCT_TYPE, float>(it->first, it->second));

    std::sort(sortIds.begin(), sortIds.end(), [](pair<PRODUCT_TYPE, float> const & left, pair<PRODUCT_TYPE, float> const & right) {
        return left.second < right.second;
    });

    for (auto id = sortIds.begin(); id != sortIds.end(); ++id)
        recommendedProducts.push_back(pair<PRODUCT_TYPE, wstring>(id->first, dataSource->Data()->productInfos[id->first]));

    return recommendedProducts;
}


int main()
{

    cout << " --- MACHINE LEARNING SERVER ---" << endl << endl;

    try
    {
        //dataSource = new core::data::CvsDataSource("C:\\Users\\ErhanBARIS\\Downloads\\ml-latest-small\\movies.csv", "C:\\Users\\ErhanBARIS\\Downloads\\ml-latest-small\\ratings.csv");
        dataSource = new core::data::CvsDataSource <core::data::DataInfo > ("/Users/erhanbaris/Downloads/ml-20m/movies.csv", "/Users/erhanbaris/Downloads/ml-20m/ratings.csv");
        //dataSource = new core::data::MongoDbDataSource("mongodb://localhost:27017");
        dataSource->LoadData();

        cout <<"Data Load Success" <<endl;

        mDistance.SetProductIndex(&dataSource->Data()->productMap);
        mDistance.SetUserIndex(&dataSource->Data()->userMap);

        crow::SimpleApp app;

        CROW_ROUTE(app, "/")
                ([]() {
                    return "<b>/api/recommend</b><br /><b>/system/refresh</b>";
                });


        CROW_ROUTE(app, "/system/refresh")
                ([]() {
                    dataSource->LoadData();
                    return "OK";
                });

        CROW_ROUTE(app, "/api/recommend/<int>")
                ([](USER_TYPE userId) {
                    vector<pair<PRODUCT_TYPE, wstring>> results;

#ifdef ENABLE_CACHE
                    auto cacheCheck = dataSource->Data()->recommendCacheForUser.find(userId);
	if (cacheCheck != dataSource->Data()->recommendCacheForUser.end())
		results = cacheCheck->second;
	else {
#endif
                    results = recommend(userId);
                    dataSource->Data()->recommendCacheForUser[userId] =  results;
#ifdef ENABLE_CACHE
                    }
#endif

                    std::stringstream stream;
                    stream << "[";

                    auto end = results.cend();
                    for (auto it = results.cbegin(); it != end; ++it)
                    {
                        string tmpName;
                        tmpName.assign(it->second.begin(), it->second.end());

                        stream << "{\"Id\":" << it->first << ",\"Name\":\"" << escapeJsonString(tmpName) << "\"},";
                    }

                    stream.seekg(0, ios::end);
                    int size = stream.tellg();

                    if (size > 1)
                        stream.seekp(-1, stream.cur);

                    stream << "]";
                    crow::response res(stream.str());
                    res.set_header("Content-Type", "application/json");
                    return res;
                });

        app.port(HTTP_SERVER_PORT).multithreaded().run();

    }
    catch (std::exception& e)
    {
        cout << "[ FATAL ERROR ] : " << e.what() << endl;
    }

    getchar();
    return 0;
}

