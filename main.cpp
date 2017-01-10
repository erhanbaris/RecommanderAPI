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

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>

//#include "crow.h"

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

using namespace ::pplx;
using namespace utility;
using namespace concurrency::streams;
using namespace web::http;


core::data::BaseDataSource<core::data::DataInfo> *dataSource;
PearsonCorrelationCoefficien mDistance;

typedef struct sRequestInfo {
    sRequestInfo(http_request & pRequest, std::map<utility::string_t, utility::string_t> pQueries) :Request(pRequest), Queries(pQueries)
    { }
    http_request Request;
    std::map<utility::string_t, utility::string_t> Queries;
} RequestInfo;

typedef std::function<http_response(RequestInfo)> FuncDelegate;
typedef CUSTOM_MAP<string, FuncDelegate>  HttpMethodRoute;
CUSTOM_MAP<string, HttpMethodRoute> routeTable;

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

http_response mainPage(RequestInfo info)
{
    http_response returnValue;
    returnValue.set_status_code(status_codes::OK);
    returnValue.set_body("<b>/api/recommend</b><br /><b>/system/refresh</b>");
    returnValue.headers().set_content_type("text/html");
    return returnValue;
}

http_response apiRecommend(RequestInfo info)
{
    if (info.Queries.find("userid") == info.Queries.end())
    {
        web::json::value item = web::json::value::object();
        item[U("Status")] = web::json::value::boolean(false);
        item[U("ErrorMessage")] = web::json::value::string("Please set 'userid' parameter");

        http_response returnValue;
        returnValue.set_status_code(status_codes::BadRequest);
        returnValue.set_body(item);
        returnValue.headers().set_content_type("application/json");

        return returnValue;
    }


    vector<pair<PRODUCT_TYPE, wstring>> results;

#ifdef ENABLE_CACHE
    auto cacheCheck = dataSource->Data()->recommendCacheForUser.find(userId);
    if (cacheCheck != dataSource->Data()->recommendCacheForUser.end())
        results = cacheCheck->second;
    else {
#endif
    results = recommend(stoi(info.Queries["userid"]));
    dataSource->Data()->recommendCacheForUser[stoi(info.Queries["userid"])] = results;
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

    http_response returnValue;
    returnValue.set_status_code(status_codes::OK);
    returnValue.set_body(stream.str());
    returnValue.headers().set_content_type("application/json");

    return returnValue;
}

http_response refreshDataSource(RequestInfo info)
{
    dataSource->LoadData();

    http_response returnValue;
    returnValue.set_status_code(status_codes::OK);
    returnValue.set_body("Loaded All Data");

    return returnValue;
}

void addHttpMethod(string  method)
{
    HttpMethodRoute route;
    INIT_MAP(route, "-1", "-2");
    routeTable[method] = route;
}

void executeRequest(web::http::http_request request, string methodType)
{
    auto relativeUri = request.relative_uri();
    auto relativePath = relativeUri.path();
    auto queries = relativeUri.split_query(relativeUri.query());
    string decode = web::http::uri::decode(relativePath);
    std::transform(decode.begin(), decode.end(), decode.begin(), ::tolower);

    if (routeTable[methodType].find(decode) != routeTable[methodType].end())
    {
        RequestInfo info(request, queries);
        http_response response = routeTable[methodType][decode](info);

        request.reply(response);
    }
    else
        request.reply(web::http::status_codes::NotFound, ("API NOT FOUND"));
}

int main(int argc, char **args) {

    cout << args[0] << endl;
    //StaticFileParser::StaticFolder = "/Users/erhanbaris/ClionProjects/RecommanderAPI/www/static";
    cout << " --- MACHINE LEARNING SERVER ---" << endl << endl;
    INIT_MAP(routeTable, "", "0");

    try {
        //dataSource = new core::data::CvsDataSource("C:\\Users\\ErhanBARIS\\Downloads\\ml-latest-small\\movies.csv", "C:\\Users\\ErhanBARIS\\Downloads\\ml-latest-small\\ratings.csv");
        dataSource = new core::data::CvsDataSource<core::data::DataInfo>(
                "/Users/erhanbaris/Downloads/ml-20m/movies.csv", "/Users/erhanbaris/Downloads/ml-20m/ratings.csv");
        //dataSource = new core::data::MongoDbDataSource("mongodb://localhost:27017");
        dataSource->LoadData();

        cout << "Data Load Success" << endl;

        mDistance.SetProductIndex(&dataSource->Data()->productMap);
        mDistance.SetUserIndex(&dataSource->Data()->userMap);


        addHttpMethod(web::http::methods::GET);
        addHttpMethod(web::http::methods::PUT);
        addHttpMethod(web::http::methods::DEL);
        addHttpMethod(web::http::methods::HEAD);
        addHttpMethod(web::http::methods::POST);

        routeTable[methods::POST].insert(make_pair("/api/user_recommend", &apiRecommend));
        routeTable[methods::GET].insert(make_pair("/api/user_recommend", &apiRecommend));
        routeTable[methods::PUT].insert(make_pair("/api/user_recommend", &apiRecommend));
        routeTable[methods::GET].insert(make_pair("/system/refresh", &refreshDataSource));
        routeTable[methods::GET].insert(make_pair("/", &mainPage));


        auto address = "http://127.0.0.1:5050";
        web::uri_builder uri(address);
        std::string addr = uri.to_uri().to_string();
        web::http::experimental::listener::http_listener listener(addr);

        listener.support(web::http::methods::POST, [](web::http::http_request request) { executeRequest(request, web::http::methods::POST); });
        listener.support(web::http::methods::GET, [](web::http::http_request request) { executeRequest(request, web::http::methods::GET); });
        listener.support(web::http::methods::PUT, [](web::http::http_request request) { executeRequest(request, web::http::methods::PUT); });
        listener.support(web::http::methods::DEL, [](web::http::http_request request) { executeRequest(request, web::http::methods::DEL); });
        listener.support(web::http::methods::HEAD, [](web::http::http_request request) { executeRequest(request, web::http::methods::HEAD); });

        listener
                .open()
                .then([]() {std::cout << "#API Init Finished" << endl << "Api Server Listening on http://localhost:5050" << "/\n"; })
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

