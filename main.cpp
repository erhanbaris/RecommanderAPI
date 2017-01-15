#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <iostream>

#pragma warning(disable:4503)

#include "config.h"
#include <sstream>
#include <map>
#include <memory>

#include "core/algoritm/PearsonCorrelationCoefficien.h"
#include "core/server/AppServer.h"
#include "core/server/action/BaseAction.h"
#include "core/server/action/UserRecommendAction.h"

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
    returnValue.Data = "Loaded All Data";

    return returnValue;
}


int main(int argc, char **args) {

    cout << args[0] << endl;
    HtmlFolder = "/Users/erhanbaris/ClionProjects/RecommanderAPI/www";
    StaticFolder = "/Users/erhanbaris/ClionProjects/RecommanderAPI/www/static";
    cout << " --- MACHINE LEARNING SERVER ---" << endl << endl;

    try {
        AppServer::instance().AddAction(new UserRecommendAction());
        AppServer::instance().Start();

    }
    catch (std::exception &e) {
        cout << "[ FATAL ERROR ] : " << e.what() << endl;
    }

    getchar();
    cout << "get chaa" << endl;
    return 0;
}

