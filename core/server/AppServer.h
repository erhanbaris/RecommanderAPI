#pragma once

#include <memory>

#include "../../config.h"
#include "../../model/Rating.h"
#include "../data/BaseDataSource.h"
#include "../data/CvsDataSource.h"
#include "../algoritm/SlopeOne.h"
#include "../MimeType.h"
#include "../algoritm/PearsonCorrelationCoefficien.h"
#include "../Utils.h"
#include "../data/BaseDataInfo.h"
#include "../data/GeneralDataInfo.h"
#include "ServerConfig.h"
#include "action/BaseAction.h"
#include "handler/StaticFileHandler.h"
#include "handler/HtmlHandler.h"
#include "handler/ActionHandler.h"

using namespace core::server;
using namespace core::server::action;

namespace core
{
    namespace server
    {
        class AppServer {
        public:
            void Start();
            std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>> DataSource();
            AppServer& AddAction(BaseAction * action);

            AppServer& SetHtmlPath(string path);
            AppServer& SetStaticPath(string path);
            AppServer& SetExecutionPath(string path);
            AppServer& SetDataPath(string path);

            string GetHtmlPath();
            string GetStaticPath();
            string GetExecutionPath();
            string GetDataPath();

            static AppServer& instance()
            {
                static AppServer INSTANCE;
                return INSTANCE;
            }

        private:
            PearsonCorrelationCoefficien mDistance;
            string dataPath;
            string executionPath;
            string htmlPath;
            string staticPath;

            AppServer();
            void ExecuteRequest(http_request * request, string const &methodType);
            std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>> dataSource;
            std::vector<core::server::handler::BaseHandler*> handlers;
            std::vector<core::server::action::BaseAction*> actions;
            std::shared_ptr<web::http::experimental::listener::http_listener> listener;
        };
    }
}