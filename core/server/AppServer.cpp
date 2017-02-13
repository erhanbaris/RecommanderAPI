#include <core/server/AppServer.h>

using namespace core;
using namespace core::server;
using namespace core::server::handler;
using namespace core::data;

void AppServer::ExecuteRequest(http_request *request, STR_TYPE const &methodType) {

    auto handlersEnd = handlers.end();
    auto relativeUri = request->relative_uri();
    auto relativePath = relativeUri.path();
    auto queries = relativeUri.split_query(relativeUri.query());
    STR_TYPE decode = web::http::uri::decode(relativePath);
    std::transform(decode.begin(), decode.end(), decode.begin(), ::tolower);
    RequestInfo info(queries, decode);
    info.MethodType = methodType;

    for (auto it = handlers.begin(); it != handlersEnd; ++it) {
        bool result = (*it)->TryExecute(&info);
        if (result) {
            request->reply((status_code) info.Response.Status, info.Response.Data, GET_WSTRING(info.Response.ContentType));
            return;
        }
    }

    request->reply(404, STR("Not Found"));
}

AppServer::AppServer() { }

void AppServer::Start() {
    try {
        dataSource = std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>>(new core::data::CvsDataSource<core::data::GeneralDataInfo>(GetDataPath() + "/cvs/products.csv", GetDataPath() + "/cvs/ratings.csv"));
        dataSource->LoadData();

        ActionHandler *actionHandler = new ActionHandler();

        auto actionsEnd = actions.end();
        for (auto it = actions.begin(); it != actionsEnd; ++it) {
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

        LOG_WRITE(STR("Data Load Success"));

        mDistance.SetProductIndex(&dataSource->Data()->productMap);
        mDistance.SetUserIndex(&dataSource->Data()->userMap);


        auto address = STR("http://localhost:") + GET_WSTRING(std::to_string(HTTP_SERVER_PORT));
        web::uri_builder uri(address);
        STR_TYPE addr = uri.to_uri().to_string();
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
                    LOG_WRITE(STR("API Init Finished"));
                    LOG_WRITE(STR("Api Server Listening on http://localhost:") << HTTP_SERVER_PORT);
                })
                .wait();
    }
    catch (std::exception &e) {
        ERROR_WRITE(e.what());
    }

	cin.get();
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
