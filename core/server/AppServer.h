#pragma once

#include <memory>

#include <config.h>
#include <model/Rating.h>
#include <core/data/BaseDataSource.h>
#include <core/data/CvsDataSource.h>
#include <core/algoritm/SlopeOne.h>
#include <core/MimeType.h>
#include <core/algoritm/PearsonCorrelationCoefficien.h>
#include <core/Utils.h>
#include <core/data/BaseDataInfo.h>
#include <core/data/GeneralDataInfo.h>
#include <core/server/ServerConfig.h>
#include <core/server/action/BaseAction.h>
#include <core/server/handler/StaticFileHandler.h>
#include <core/server/handler/HtmlHandler.h>
#include <core/server/handler/ActionHandler.h>

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

            AppServer& SetHtmlPath(STR_TYPE path);
            AppServer& SetStaticPath(STR_TYPE path);
            AppServer& SetExecutionPath(STR_TYPE path);
            AppServer& SetDataPath(STR_TYPE path);

            STR_TYPE GetHtmlPath();
            STR_TYPE GetStaticPath();
            STR_TYPE GetExecutionPath();
            STR_TYPE GetDataPath();

            static AppServer& instance()
            {
                static AppServer INSTANCE;
                return INSTANCE;
            }

        private:
            PearsonCorrelationCoefficien mDistance;
            STR_TYPE dataPath;
            STR_TYPE executionPath;
            STR_TYPE htmlPath;
            STR_TYPE staticPath;

            AppServer();
            void ExecuteRequest(http_request * request, STR_TYPE const &methodType);
            std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>> dataSource;
            std::vector<core::server::handler::BaseHandler*> handlers;
            std::vector<core::server::action::BaseAction*> actions;
            std::shared_ptr<web::http::experimental::listener::http_listener> listener;
        };
    }
}