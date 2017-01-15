#include <core/server/handler/StaticFileHandler.h>
#include <core/Utils.h>
#include <core/server/AppServer.h>

using namespace core;
using namespace core::server;
using namespace core::server::handler;

namespace {
    core::MimeType mimeType;
}

StaticFileHandler::StaticFileHandler() {
    INIT_MAP(fileCache, "", " ");
};

bool StaticFileHandler::TryExecute(RequestInfo * request) {
    bool returnValue = false;

    try {
        const char *staticName = "/static/";
        auto isStaticFile = strncmp(request->Url.c_str(), staticName, strlen(staticName)) == 0;
        DEBUG_WRITE(request->Url);
        
        if (isStaticFile) {
            returnValue = true;
            
            string url = request->Url;
            url.replace(url.begin(), url.end(), staticName);
            string fullFilePath = AppServer::instance().GetStaticPath() + "/" + url;

            if (fileExists(fullFilePath)) {

                try {
                    
                    auto dotLocation = url.find(".") + 1;
                    string fileType = url.substr(dotLocation);

                    std::ifstream ifs(fullFilePath);
                    string str(static_cast<std::stringstream const &>(std::stringstream() << ifs.rdbuf()).str());
                    ifs.close();

                    request->Response.Data = str;
                    request->Response.Status = status_codes::OK;
                    request->Response.ContentType = mimeType.GetMimeType(fileType);
                } catch (std::exception &e) {
                    request->Response.Data = "Internal Server Error";
                    request->Response.Status = status_codes::InternalError;
                    request->Response.ContentType = "text/html";
                }
            }
            else{
                request->Response.Data = "Not Found";
                request->Response.Status = status_codes::NotFound;
                request->Response.ContentType = "text/html";
            }
        }
    }
    catch (std::exception &e) {
        ERROR_WRITE(e.what());
    }

    return returnValue;
}
