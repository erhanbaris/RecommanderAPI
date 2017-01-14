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
#include "core/MimeType.h"
#include <vector>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <functional>
#include <locale>
#include <sstream>
#include <map>

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/producerconsumerstream.h>
#include <cpprest/json.h>
#include <cpprest/interopstream.h>

#include "core/algoritm/PearsonCorrelationCoefficien.h"
#include "model/Rating.h"
#include "core/types.h"
#include "core/data/BaseDataInfo.h"
#include "core/data/GeneralDataInfo.h"
#include "core/server/ServerConfig.h"
#include "core/server/handler/BaseHandler.h"
#include "core/server/handler/StaticFileHandler.h"
#include "core/server/handler/HtmlHandler.h"
#include "core/server/handler/ActionHandler.h"

#pragma execution_character_set("utf-8")

using namespace std;
using namespace std::chrono;
using namespace model;
using namespace core::algoritm;
using namespace core::server;
using namespace core::server::handler;

using namespace utility;
using namespace concurrency::streams;
using namespace web::http;

using namespace web;
using namespace web::http;
using namespace web::http::client;


core::data::BaseDataSource<core::data::GeneralDataInfo> *dataSource;
PearsonCorrelationCoefficien mDistance;


std::string escapeJsonString(const std::string &input) {
    std::ostringstream ss;
    for (auto iter = input.cbegin(); iter != input.cend(); iter++) {
        //C++98/03:
        //for (std::string::const_iterator iter = input.begin(); iter != input.end(); iter++) {
        switch (*iter) {
            case '\\':
                ss << "\\\\";
                break;
            case '"':
                ss << "\\\"";
                break;
            case '/':
                ss << "\\/";
                break;
            case '\b':
                ss << "\\b";
                break;
            case '\f':
                ss << "\\f";
                break;
            case '\n':
                ss << "\\n";
                break;
            case '\r':
                ss << "\\r";
                break;
            case '\t':
                ss << "\\t";
                break;
            default:
                ss << *iter;
                break;
        }
    }
    return ss.str();
}

vector<pair<PRODUCT_TYPE, wstring>> recommend(USER_TYPE userId) {
    vector<pair<PRODUCT_TYPE, wstring>> recommendedProducts;
    CUSTOM_MAP<PRODUCT_TYPE, float> recommendedProductIds;
    INT_INIT_MAP(recommendedProductIds);

    CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> userRatings;
    INT_INIT_MAP(userRatings);

    auto &userDatas = dataSource->Data()->userMap[userId].Products;
    auto endData = userDatas.end();
    for (auto it = userDatas.begin(); it != endData; ++it)
        userRatings[(*it)->ProductId] = (*it)->Rating;

    const auto &results = dataSource->Data()->slopeOne.Predict(userRatings);
    //const auto& results = mDistance.Recommend(&dataSource->Data()->userRatings, userId);
    if (results.size() > 0) {
        auto nearestUser = results.at(0);

        auto nearestUserRatings = dataSource->Data()->userRatings[nearestUser.first];
        auto currentUserRatings = dataSource->Data()->userRatings[userId];
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
            auto neighborRatings = dataSource->Data()->userRatings[nearest->first];

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
        recommendedProducts.push_back(
                pair<PRODUCT_TYPE, wstring>(id->first, dataSource->Data()->productInfos[id->first]));

    return recommendedProducts;
}

ResponseInfo mainPage(RequestInfo *info) {
    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = "<b>/api/recommend</b><br /><b>/system/refresh</b>";
    returnValue.ContentType = "text/html";
    return returnValue;
}

ResponseInfo apiRecommend(RequestInfo *info) {
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
    auto cacheCheck = dataSource->Data()->recommendCacheForUser.find(userId);
    if (cacheCheck != dataSource->Data()->recommendCacheForUser.end())
        results = cacheCheck->second;
    else {
#endif
    results = recommend(stoi(info->Queries["userid"]));
    dataSource->Data()->recommendCacheForUser[stoi(info->Queries["userid"])] = results;
#ifdef ENABLE_CACHE
    }
#endif

    std::stringstream stream;
    stream << "[";

    auto end = results.cend();
    for (auto it = results.cbegin(); it != end; ++it) {
        string tmpName;
        tmpName.assign(it->second.begin(), it->second.end());

        stream << "{\"Id\":" << it->first << ",\"Name\":\"" << escapeJsonString(tmpName) << "\"},";
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

ResponseInfo refreshDataSource(RequestInfo *info) {
    dataSource->LoadData();

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = "Loaded All Data";

    return returnValue;
}

std::vector<core::server::handler::BaseHandler *> handlers;

void executeRequest(http_request *request, string const &methodType) {

    auto relativeUri = request->relative_uri();
    auto relativePath = relativeUri.path();
    auto queries = relativeUri.split_query(relativeUri.query());
    string decode = web::http::uri::decode(relativePath);
    std::transform(decode.begin(), decode.end(), decode.begin(), ::tolower);
    RequestInfo info(decode);
    info.MethodType = methodType;

    for (auto it = handlers.begin(); it != handlers.end(); ++it) {
        bool result = (*it)->TryExecute(&info);
        if (result) {
            request->reply(info.Response.Status, info.Response.Data, info.Response.ContentType);
            break;
        }
    }
    string _404 = "Not Found";
    request->reply(404, _404);
}

void handle_get(http_request request) {
    auto relativeUri = request.relative_uri();
    auto relativePath = relativeUri.path();
    auto queries = relativeUri.split_query(relativeUri.query());
    string decode = web::http::uri::decode(relativePath);
    std::transform(decode.begin(), decode.end(), decode.begin(), ::tolower);
    RequestInfo info(decode);
    info.MethodType = web::http::methods::GET;

    for (auto it = handlers.begin(); it != handlers.end(); ++it) {

        bool result = (*it)->TryExecute(&info);
        if (result) {
            request.reply(info.Response.Status, info.Response.Data, info.Response.ContentType).wait();
            break;
        }
    }

    request.reply(404, "Not Found").wait();

}

int main(int argc, char **args) {

    cout << args[0] << endl;
    HtmlFolder = "/Users/erhanbaris/ClionProjects/RecommanderAPI/www";
    StaticFolder = "/Users/erhanbaris/ClionProjects/RecommanderAPI/www/static";
    cout << " --- MACHINE LEARNING SERVER ---" << endl << endl;

    try {
        dataSource = new core::data::CvsDataSource<core::data::GeneralDataInfo>(
                "/Users/erhanbaris/Downloads/ml-20m/movies.csv", "/Users/erhanbaris/Downloads/ml-20m/ratings.csv");
        dataSource->LoadData();

        ActionHandler *actionHandler = new ActionHandler();

        actionHandler->SetPostAction("/api/user_recommend", apiRecommend);
        actionHandler->SetGetAction("/api/user_recommend", apiRecommend);
        actionHandler->SetGetAction("/api/user_recommend", apiRecommend);
        actionHandler->SetGetAction("/system/refresh", refreshDataSource);

        //handlers.push_back(new StaticFileHandler());
        handlers.push_back(new HtmlHandler());
        //handlers.push_back(actionHandler);

        cout << "Data Load Success" << endl;

        mDistance.SetProductIndex(&dataSource->Data()->productMap);
        mDistance.SetUserIndex(&dataSource->Data()->userMap);


        auto address = "http://127.0.0.1:5050";
        web::uri_builder uri(address);
        std::string addr = uri.to_uri().to_string();
        web::http::experimental::listener::http_listener listener(addr);

        listener.support(web::http::methods::POST, [](web::http::http_request request) {
            return executeRequest(&request, web::http::methods::POST);
        });
        //listener.support(web::http::methods::GET, [](web::http::http_request request) { return executeRequest(&request, web::http::methods::GET); });
        listener.support(web::http::methods::GET, handle_get);
        listener.support(web::http::methods::PUT, [](web::http::http_request request) {
            return executeRequest(&request, web::http::methods::PUT);
        });
        listener.support(web::http::methods::DEL, [](web::http::http_request request) {
            return executeRequest(&request, web::http::methods::DEL);
        });
        listener.support(web::http::methods::HEAD, [](web::http::http_request request) {
            return executeRequest(&request, web::http::methods::HEAD);
        });

        listener
                .open()
                .then([]() {
                    std::cout << "#API Init Finished" << endl << "Api Server Listening on http://localhost:5050" <<
                    "/\n";
                })
                .wait();

        std::string lineread;
        std::getline(std::cin, lineread);

        listener.close().wait();
    }
    catch (std::exception &e) {
        cout << "[ FATAL ERROR ] : " << e.what() << endl;
    }

    getchar();
    cout << "get chaa" << endl;
    return 0;
}

