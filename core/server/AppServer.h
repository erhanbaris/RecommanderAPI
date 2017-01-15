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
            void ExecuteRequest(http_request * request, string const &methodType);
            std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>> DataSource();
            void AddAction(BaseAction * action);

            static AppServer& instance()
            {
                static AppServer INSTANCE;
                return INSTANCE;
            }

        private:
            PearsonCorrelationCoefficien mDistance;
            AppServer();
            std::shared_ptr<core::data::CvsDataSource<core::data::GeneralDataInfo>> dataSource;
            std::vector<core::server::handler::BaseHandler*> handlers;
            std::vector<core::server::action::BaseAction*> actions;
            std::shared_ptr<web::http::experimental::listener::http_listener> listener;
        };
    }
}