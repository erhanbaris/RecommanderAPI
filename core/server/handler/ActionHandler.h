#pragma once

#include <functional>

#include <config.h>

#include <core/server/ServerConfig.h>
#include <core/server/handler/StaticFileHandler.h>
#include <core/server/action/BaseAction.h>

using namespace core::server::action;

namespace core {
    namespace server {
        namespace handler {

            typedef CUSTOM_MAP<string, BaseAction*> HttpMethodRoute;

            class ActionHandler : public BaseHandler {
            public:
                ActionHandler();

                bool TryExecute(RequestInfo * request) override;
                void AddHttpMethod(string method);

                void SetGetAction(string url, BaseAction* action);
                void SetPostAction(string url, BaseAction* action);
                void SetDeleteAction(string url, BaseAction* action);
                void SetPutAction(string url, BaseAction* action);
                void SetHeadAction(string url, BaseAction* action);

            private:
                CUSTOM_MAP<string, HttpMethodRoute> routeTable;
            };
        }
    }
}
