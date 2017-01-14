#include "StaticFileHandler.h"

using namespace core::server::handler;

namespace {
    core::MimeType mimeType;

    long getFileSize(FILE *file) {
        long lCurPos, lEndPos;
        lCurPos = ftell(file);
        fseek(file, 0, 2);
        lEndPos = ftell(file);
        fseek(file, lCurPos, 0);
        return lEndPos;

    }

    bool fileExists(string const &filename) {
        if (FILE *file = fopen(filename.c_str(), "r")) {
            fclose(file);
            return true;
        }

        return false;
    }

    size_t indexOf(char *text, char searchChar) {
        size_t totalLength = strlen(text);
        char *e = text + totalLength;
        size_t idx = 0;
        while (*e-- != searchChar && totalLength != idx)
            idx++;

        return totalLength - idx;
    }
}

StaticFileHandler::StaticFileHandler() {
    INIT_MAP(fileCache, "", " ");
};

bool StaticFileHandler::TryExecute(RequestInfo * request) {
    bool returnValue = false;

    try {
        const char *staticName = "/static/";
        auto isStaticFile = strncmp(request->Url.c_str(), staticName, strlen(staticName)) == 0;
        if (isStaticFile) {
            returnValue = true;

            size_t filenameLength = request->Url.size() - strlen(staticName);
            char *filePath = new char[filenameLength];
            strcpy(filePath, request->Url.c_str() + strlen(staticName));

            char *fullFilePath = new char[strlen(filePath) + StaticFolder.size()];

            strcat(fullFilePath, StaticFolder.c_str());
            strcat(fullFilePath, "/");
            strcat(fullFilePath, filePath);

            if (fileExists(fullFilePath)) {

                try {
                    auto dotLocation = indexOf(filePath, '.') + 1;
                    char *fileType = new char[filenameLength - dotLocation];
                    strcpy(fileType, filePath + dotLocation);


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
        cout << e.what() << endl;
    }

    return returnValue;
}