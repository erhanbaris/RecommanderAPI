#include "ActionHandler.h"

using namespace core::server::handler;


ActionHandler::ActionHandler()
{
    INIT_MAP(routeTable, "", "0");

    addHttpMethod(web::http::methods::GET);
    addHttpMethod(web::http::methods::PUT);
    addHttpMethod(web::http::methods::DEL);
    addHttpMethod(web::http::methods::HEAD);
    addHttpMethod(web::http::methods::POST);
}

bool ActionHandler::TryExecute(RequestInfo * request) {
    if (routeTable[request->MethodType].find(request->Url) != routeTable[request->MethodType].end()) {
        auto response = routeTable[request->MethodType][request->Url](request);

        request->Response.Data = response.Data;
        request->Response.Status = response.Status;
        request->Response.ContentType = response.ContentType;
    }
    else
    {
        request->Response.Data = "Not Found";
        request->Response.Status = status_codes::NotFound;
        request->Response.ContentType = "text/html";
    }

    return true;
};

void ActionHandler::addHttpMethod(string method) {
    HttpMethodRoute route;
    INIT_MAP(route, "-1", "-2");
    routeTable[method] = route;
}

void ActionHandler::SetGetAction(string url, FuncDelegate delegate)
{
    routeTable[methods::GET].insert(std::make_pair(url, delegate));
}

void ActionHandler::SetPostAction(string url, FuncDelegate delegate)
{
    routeTable[methods::POST].insert(std::make_pair(url, delegate));
}

void ActionHandler::SetDeleteAction(string url, FuncDelegate delegate)
{
    routeTable[methods::DEL].insert(std::make_pair(url, delegate));
}

void ActionHandler::SetPutAction(string url, FuncDelegate delegate)
{
    routeTable[methods::PUT].insert(std::make_pair(url, delegate));
}

void ActionHandler::SetHeadAction(string url, FuncDelegate delegate)
{
    routeTable[methods::HEAD].insert(std::make_pair(url, delegate));
}