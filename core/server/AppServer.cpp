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
            request->reply((status_code) info.Response.Status, info.Response.Data, info.Response.ContentType);
            return;
        }
    }

    request->reply(404, "Not Found");
}

AppServer::AppServer() { }

void AppServer::Start() {
    try {
        dataSource = std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>>(new core::data::CvsDataSource<core::data::GeneralDataInfo>(GetDataPath() + STR("/cvs/products.csv"), GetDataPath() + STR("/cvs/ratings.csv")));
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


        auto address = STR("http://0.0.0.0:") + std::to_string(HTTP_SERVER_PORT);
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
                    LOG_WRITE(STR("Api Server Listening on http://0.0.0.0:") << HTTP_SERVER_PORT);
                })
                .wait();
    }
    catch (std::exception &e) {
        ERROR_WRITE(e.what());
    }

    STR_TYPE lineread;
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

AppServer& AppServer::SetHtmlPath(STR_TYPE path)
{
    htmlPath = path;
    return *this;
}

AppServer& AppServer::SetStaticPath(STR_TYPE path)
{
    staticPath = path;
    return *this;
}

AppServer& AppServer::SetExecutionPath(STR_TYPE path)
{
    executionPath = path;
    return *this;
}

AppServer& AppServer::SetDataPath(STR_TYPE path)
{
    dataPath = path;
    return *this;
}

STR_TYPE AppServer::GetHtmlPath()
{
    return dataPath + STR("/") + htmlPath;
}

STR_TYPE AppServer::GetStaticPath()
{
    return dataPath + STR("/") + staticPath;
}

STR_TYPE AppServer::GetExecutionPath()
{
    return executionPath;
}

STR_TYPE AppServer::GetDataPath()
{
    return dataPath;
}
