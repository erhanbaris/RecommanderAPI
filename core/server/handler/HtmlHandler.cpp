#include "HtmlHandler.h"

using namespace core::server::handler;

namespace {
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

    size_t filenameLength = searchUrl.size();

    string fullFilePath;

    fullFilePath = HtmlFolder + "/" + searchUrl;

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
