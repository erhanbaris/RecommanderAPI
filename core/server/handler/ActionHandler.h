#pragma once

#include <functional>

#include <config.h>

#include <core/server/ServerConfig.h>
#include <core/server/handler/BaseHandler.h>
#include <core/server/action/BaseAction.h>

using namespace core::server;
using namespace core::server::action;

namespace core {
    namespace server {
        namespace handler {

            typedef CUSTOM_MAP<STR_TYPE, BaseAction*> HttpMethodRoute;

            class ActionHandler : public BaseHandler {
            public:
                ActionHandler();

                bool TryExecute(RequestInfo * request) override;
                void AddHttpMethod(STR_TYPE method);

                void SetGetAction(STR_TYPE url, BaseAction* action);
                void SetPostAction(STR_TYPE url, BaseAction* action);
                void SetDeleteAction(STR_TYPE url, BaseAction* action);
                void SetPutAction(STR_TYPE url, BaseAction* action);
                void SetHeadAction(STR_TYPE url, BaseAction* action);

            private:
                CUSTOM_MAP<STR_TYPE, HttpMethodRoute> routeTable;
            };
        }
    }
}
