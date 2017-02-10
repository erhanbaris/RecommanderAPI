#include <iostream>
#include <fstream>

#pragma warning(disable:4503)

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <direct.h>
#define GetCurrentDir _getcwd
#else

#include <unistd.h>

#define GetCurrentDir getcwd
#endif

#include <config.h>
#include <sstream>
#include <map>

#include <core/server/AppServer.h>
#include <core/server/action/UserRecommendAction.h>
#include <core/server/action/SearchAction.h>

#pragma execution_character_set("utf-8")

using namespace std;
using namespace std::chrono;
using namespace model;
using namespace core::algoritm;
using namespace core::server;
using namespace core::server::handler;
using namespace core::server::action;

using namespace utility;
using namespace concurrency::streams;
using namespace web::http;

using namespace web;
using namespace web::http;
using namespace web::http::client;

ResponseInfo refreshDataSource(RequestInfo *info) {
    //dataSource->LoadData();

    ResponseInfo returnValue;
    returnValue.Status = status_codes::OK;
    returnValue.Data = STR("Loaded All Data");

    return returnValue;
}

int main(int argc, char **args) {
    char currentPathChars[FILENAME_MAX];
    if (!GetCurrentDir(currentPathChars, sizeof(currentPathChars)))
        return errno;

    currentPathChars[sizeof(currentPathChars) - 1] = '\0';
    string currentPath(currentPathChars);
    string dataPath;

    if (argc > 1)
        dataPath = args[1];
    else
        dataPath = currentPath;

    try {
        AppServer::instance()
                .AddAction(new UserRecommendAction())
                .AddAction(new SearchAction())
                .SetExecutionPath(currentPath)
                .SetDataPath(dataPath)
                .SetHtmlPath("www")
                .SetStaticPath("www/static");
        AppServer::instance().Start();
    }
    catch (std::exception &e) {
        ERROR_WRITE(e.what());

    }

    getchar();
    LOG_WRITE(STR("RECOMMENDER EXIT"));
    return 0;
}

