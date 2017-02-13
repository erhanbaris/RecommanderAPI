#include <core/server/handler/HtmlHandler.h>
#include <core/Utils.h>
#include <core/server/AppServer.h>

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
    auto searchUrl = GET_STRING(request->Url);

    if (fileAliases.find(searchUrl) != fileAliases.end())
        searchUrl = fileAliases.find(searchUrl)->second;

    string fullFilePath = AppServer::instance().GetHtmlPath() + searchUrl;
    DEBUG_WRITE(fullFilePath);

    if (fileExists(fullFilePath)) {
        try {
            IFSTREAM_TYPE ifs(fullFilePath);
            STRSTREAM_TYPE stringStream;
            stringStream << ifs.rdbuf();

            request->Response.Data = stringStream.str();
            request->Response.Status = status_codes::OK;
            request->Response.ContentType = "text/html";

            ifs.close();

            returnValue = true;
        } catch (std::exception &e) {
			ERROR_WRITE(e.what());
            request->Response.Data = STR("Internal Server Error");
            request->Response.Status = status_codes::InternalError;
            request->Response.ContentType = "text/html";
        }
    }

    return returnValue;
};
