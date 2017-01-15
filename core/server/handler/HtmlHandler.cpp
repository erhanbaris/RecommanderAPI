#include "HtmlHandler.h"
#include "../../Utils.h"
#include "../AppServer.h"

using namespace core;
using namespace core::server::handler;

HtmlHandler::HtmlHandler() {
    INIT_MAP(fileCache, "", " ");
    INIT_MAP(fileAliases, "", " ");

    fileAliases["/"] = "/index.html";
}

void HtmlHandler::AddAlias(string alias, string fileName) {

    if (fileName.find("/") != 0)
        fileName = "/" + fileName;

    if (alias.find("/") != 0)
        alias = "/" + alias;

    fileAliases[alias] = fileName;
}

bool HtmlHandler::TryExecute(RequestInfo * request) {

    bool returnValue = false;
    auto searchUrl = request->Url;

    if (fileAliases.find(request->Url) != fileAliases.end())
        searchUrl = fileAliases.find(request->Url)->second;

    string fullFilePath = AppServer::instance().GetHtmlPath() + searchUrl;
    DEBUG_WRITE(fullFilePath);

    if (fileExists(fullFilePath)) {
        try {
            std::ifstream ifs(fullFilePath);
            std::stringstream stringStream;
            stringStream << ifs.rdbuf();

            request->Response.Data = stringStream.str();
            request->Response.Status = status_codes::OK;
            request->Response.ContentType = "text/html";

            ifs.close();

            returnValue = true;
        } catch (std::exception &e) {
            request->Response.Data = "Internal Server Error";
            request->Response.Status = status_codes::InternalError;
            request->Response.ContentType = "text/html";
        }
    }

    return returnValue;
};
