#include <core/server/AppServer.h>

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

#include <config.h>
#include <model/Rating.h>
#include <core/data/BaseDataSource.h>
#include <core/data/CvsDataSource.h>
#include <core/algoritm/SlopeOne.h>
#include <core/MimeType.h>
#include <core/algoritm/PearsonCorrelationCoefficien.h>
#include <core/Utils.h>
#include <core/data/BaseDataInfo.h>
#include <core/data/GeneralDataInfo.h>
#include <core/server/ServerConfig.h>
#include <core/server/handler/BaseHandler.h>
#include <core/server/handler/StaticFileHandler.h>
#include <core/server/handler/HtmlHandler.h>
#include <core/server/handler/ActionHandler.h>

using namespace core;
using namespace core::server;
using namespace core::server::handler;
using namespace core::data;

void AppServer::ExecuteRequest(http_request *request, string const &methodType) {

    auto relativeUri = request->relative_uri();
    auto relativePath = relativeUri.path();
    auto queries = relativeUri.split_query(relativeUri.query());
    string decode = web::http::uri::decode(relativePath);
    std::transform(decode.begin(), decode.end(), decode.begin(), ::tolower);
    RequestInfo info(queries, decode);
    info.MethodType = methodType;

    for (auto it = handlers.begin(); it != handlers.end(); ++it) {
        bool result = (*it)->TryExecute(&info);
        if (result) {
            request->reply(info.Response.Status, info.Response.Data, info.Response.ContentType);
            return;
        }
    }

    request->reply(404, "Not Found");
}

AppServer::AppServer() { }

void AppServer::Start() {
    try {
        dataSource = std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>>(
                new core::data::CvsDataSource<core::data::GeneralDataInfo>(GetDataPath() + "/cvs/products.csv", GetDataPath() + "/cvs/ratings.csv"));
        dataSource->LoadData();

        ActionHandler *actionHandler = new ActionHandler();

        for (auto it = actions.begin(); it != actions.end(); ++it) {
            short methods = (*it)->MethodInfo();
            if (methods & TYPE_GET)
                actionHandler->SetGetAction((*it)->Url(), *it);

            if (methods & TYPE_POST)
                actionHandler->SetPostAction((*it)->Url(), *it);

            if (methods & TYPE_PUT)
                actionHandler->SetPutAction((*it)->Url(), *it);

            if (methods & TYPE_DEL)
                actionHandler->SetDeleteAction((*it)->Url(), *it);

            if (methods & TYPE_HEAD)
                actionHandler->SetHeadAction((*it)->Url(), *it);
        }

        handlers.push_back(new StaticFileHandler());
        handlers.push_back(new HtmlHandler());
        handlers.push_back(actionHandler);

        LOG_WRITE("Data Load Success");

        mDistance.SetProductIndex(&dataSource->Data()->productMap);
        mDistance.SetUserIndex(&dataSource->Data()->userMap);


        auto address = "http://127.0.0.1:" + std::to_string(HTTP_SERVER_PORT);
        web::uri_builder uri(address);
        std::string addr = uri.to_uri().to_string();
        listener = std::shared_ptr<web::http::experimental::listener::http_listener>(
                new web::http::experimental::listener::http_listener(addr));

        listener->support(web::http::methods::POST, [=](web::http::http_request request) {
            return ExecuteRequest(&request, web::http::methods::POST);
        });
        listener->support(web::http::methods::GET, [=](web::http::http_request request) {
            return ExecuteRequest(&request, web::http::methods::GET);
        });
        listener->support(web::http::methods::PUT, [=](web::http::http_request request) {
            return ExecuteRequest(&request, web::http::methods::PUT);
        });
        listener->support(web::http::methods::DEL, [=](web::http::http_request request) {
            return ExecuteRequest(&request, web::http::methods::DEL);
        });
        listener->support(web::http::methods::HEAD, [=](web::http::http_request request) {
            return ExecuteRequest(&request, web::http::methods::HEAD);
        });

        listener->open()
                .then([]() {
                    LOG_WRITE("API Init Finished");
                    LOG_WRITE("Api Server Listening on http://localhost:" << HTTP_SERVER_PORT);
                })
                .wait();
    }
    catch (std::exception &e) {
        ERROR_WRITE(e.what());
    }

    std::string lineread;
    std::getline(std::cin, lineread);

    listener->close().wait();
}

std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>> AppServer::DataSource() {
    return dataSource;
}

AppServer& AppServer::AddAction(core::server::action::BaseAction *action) {
    actions.push_back(action);
    return *this;
}

AppServer& AppServer::SetHtmlPath(string path)
{
    htmlPath = path;
    return *this;
}

AppServer& AppServer::SetStaticPath(string path)
{
    staticPath = path;
    return *this;
}

AppServer& AppServer::SetExecutionPath(string path)
{
    executionPath = path;
    return *this;
}

AppServer& AppServer::SetDataPath(string path)
{
    dataPath = path;
    return *this;
}

string AppServer::GetHtmlPath()
{
    return dataPath + "/" + htmlPath;
}

string AppServer::GetStaticPath()
{
    return dataPath + "/" + staticPath;
}

string AppServer::GetExecutionPath()
{
    return executionPath;
}

string AppServer::GetDataPath()
{
    return dataPath;
}