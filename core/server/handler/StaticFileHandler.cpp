#include <core/server/handler/StaticFileHandler.h>
#include <core/Utils.h>
#include <core/server/AppServer.h>
#include <boost/algorithm/string/replace.hpp>

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
            
            STR_TYPE url = request->Url;
            boost::replace_all(url, staticName, "");
            string fullFilePath = AppServer::instance().GetStaticPath() + STR("/") + url;

            if (fileExists(fullFilePath)) {

                try {
                    
                    auto dotLocation = url.find(STR(".")) + 1;
                    string fileType = url.substr(dotLocation);

                    std::ifstream ifs(fullFilePath);
                    STR_TYPE str(static_cast<STRSTREAM_TYPE const &>(STRSTREAM_TYPE() << ifs.rdbuf()).str());
                    ifs.close();

                    request->Response.Data = str;
                    request->Response.Status = status_codes::OK;
                    request->Response.ContentType = mimeType.GetMimeType(fileType);
                } catch (std::exception &e) {
                    request->Response.Data = STR("Internal Server Error");
                    request->Response.Status = status_codes::InternalError;
                    request->Response.ContentType = "text/html";
                }
            }
            else{
                request->Response.Data = STR("Not Found");
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
