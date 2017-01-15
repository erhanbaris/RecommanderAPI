#include <core/server/action/UserRecommendAction.h>
#include <core/server/AppServer.h>

using namespace core::server::action;


vector<pair<PRODUCT_TYPE, wstring>> UserRecommendAction::recommend(USER_TYPE userId) {
    vector<pair<PRODUCT_TYPE, wstring>> recommendedProducts;
    CUSTOM_MAP<PRODUCT_TYPE, float> recommendedProductIds;
    INT_INIT_MAP(recommendedProductIds);

    CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> userRatings;
    INT_INIT_MAP(userRatings);

    auto &userDatas = AppServer::instance().DataSource()->Data()->userMap[userId].Products;
    auto endData = userDatas.end();
    for (auto it = userDatas.begin(); it != endData; ++it)
        userRatings[(*it)->ProductId] = (*it)->Rating;

    const auto &results = AppServer::instance().DataSource()->Data()->slopeOne.Predict(userRatings);
    //const auto& results = mDistance.Recommend(&dataSource->Data()->userRatings, userId);
    if (results.size() > 0) {
        auto nearestUser = results.at(0);

        auto nearestUserRatings = AppServer::instance().DataSource()->Data()->userRatings[nearestUser.first];
        auto currentUserRatings = AppServer::instance().DataSource()->Data()->userRatings[userId];
        auto currentUserRatingsEnd = currentUserRatings.end();

        float totalDistance = 0.0;
        size_t counter = 0;
        auto resultsEnd = results.cend();
        for (auto it = results.cbegin(); it != resultsEnd && counter < KNEAR_SIZE; ++it) {
            totalDistance += it->second;
            ++counter;
        }

        counter = 0;
        for (auto nearest = results.cbegin(); nearest != resultsEnd && counter < KNEAR_SIZE; ++nearest) {
            float weight = nearest->second / totalDistance;
            auto neighborRatings = AppServer::instance().DataSource()->Data()->userRatings[nearest->first];

            auto neighborRatingsEnd = neighborRatings.cend();
            for (auto artist = neighborRatings.cbegin(); artist != neighborRatingsEnd; ++artist) {
                const auto &artistItem = (*artist);

                bool hasProduct = false;
                for (auto it = currentUserRatings.begin(); it != currentUserRatingsEnd; ++it) {
                    if (it->ProductId == artistItem.ProductId) {
                        hasProduct = true;
                        break;
                    }
                }

                if (hasProduct) {
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

    std::sort(sortIds.begin(), sortIds.end(),
              [](pair<PRODUCT_TYPE, float> const &left, pair<PRODUCT_TYPE, float> const &right) {
                  return left.second < right.second;
              });

    for (auto id = sortIds.begin(); id != sortIds.end(); ++id)
        recommendedProducts.push_back(pair<PRODUCT_TYPE, wstring>(id->first, AppServer::instance().DataSource()->Data()->productInfos[id->first]));

    return recommendedProducts;
}

string UserRecommendAction::Url() {
    return "/api/user_recommend";
}

core::server::ResponseInfo UserRecommendAction::Execute(RequestInfo *info) {
    if (info->Queries.find("userid") == info->Queries.end()) {
        web::json::value item = web::json::value::object();
        item[U("Status")] = web::json::value::boolean(false);
        item[U("ErrorMessage")] = web::json::value::string("Please set 'userid' parameter");

        ResponseInfo returnValue;
        returnValue.Status = status_codes::BadRequest;
        returnValue.Data = "";
        returnValue.ContentType = "application/json";

        return returnValue;
    }


    vector<pair<PRODUCT_TYPE, wstring>> results;

#ifdef ENABLE_CACHE
    auto cacheCheck = dataSource->Data()->recommendCacheForUser.find(stoi(info->Queries["userid"]));
    if (cacheCheck != dataSource->Data()->recommendCacheForUser.end())
        results = cacheCheck->second;
    else {
#endif
    results = recommend(stoi(info->Queries["userid"]));
    //this->appServer->DataSource()->Data()->recommendCacheForUser[stoi(info->Queries["userid"])] = results;
#ifdef ENABLE_CACHE
    }
#endif

    std::stringstream stream;
    stream << "[";

    auto end = results.cend();
    for (auto it = results.cbegin(); it != end; ++it) {
        string tmpName;
        tmpName.assign(it->second.begin(), it->second.end());

        stream << "{\"Id\":" << it->first << ",\"Name\":\"" << this->EscapeJsonString(tmpName) << "\"},";
    }

    stream.seekg(0, ios::end);
    int size = stream.tellg();

    if (size > 1)
        stream.seekp(-1, stream.cur);

    stream << "]";

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = stream.str();
    returnValue.ContentType = "application/json";

    return returnValue;
}

short UserRecommendAction::MethodInfo() {
    return TYPE_GET | TYPE_POST;
}
