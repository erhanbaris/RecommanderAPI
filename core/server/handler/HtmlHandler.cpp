#include <core/server/handler/HtmlHandler.h>
#include <core/Utils.h>
#include <core/server/AppServer.h>

using namespace core;
using namespace core::server::handler;

HtmlHandler::HtmlHandler() {
    INIT_MAP(fileCache, STR(""), STR(" "));
    INIT_MAP(fileAliases, STR(""), STR(" "));

    fileAliases[STR("/")] = STR("/index.html");
}

void HtmlHandler::AddAlias(STR_TYPE alias, STR_TYPE fileName) {

    if (fileName.find(STR("/")) != 0)
        fileName = STR("/") + fileName;

    if (alias.find(STR("/")) != 0)
        alias = STR("/") + alias;

    fileAliases[alias] = fileName;
}

bool HtmlHandler::TryExecute(RequestInfo * request) {

    bool returnValue = false;
    auto searchUrl = request->Url;

    if (fileAliases.find(request->Url) != fileAliases.end())
        searchUrl = fileAliases.find(request->Url)->second;

    STR_TYPE fullFilePath = AppServer::instance().GetHtmlPath() + searchUrl;
    DEBUG_WRITE(fullFilePath);

    if (fileExists(fullFilePath)) {
        try {
            IFSTREAM_TYPE ifs(fullFilePath);
            STRSTREAM_TYPE stringStream;
            stringStream << ifs.rdbuf();

            request->Response.Data = stringStream.str();
            request->Response.Status = status_codes::OK;
            request->Response.ContentType = STR("text/html");

            ifs.close();

            returnValue = true;
        } catch (std::exception &e) {
            request->Response.Data = STR("Internal Server Error");
            request->Response.Status = status_codes::InternalError;
            request->Response.ContentType = STR("text/html");
        }
    }

    return returnValue;
};
