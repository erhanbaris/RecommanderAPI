#pragma once

#include <functional>

#include "../../../config.h"

#include "../ServerConfig.h"
#include "StaticFileHandler.h"
#include "../action/BaseAction.h"

using namespace core::server::action;

namespace core {
    namespace server {
        namespace handler {

            typedef CUSTOM_MAP<string, BaseAction*> HttpMethodRoute;

            class ActionHandler : public BaseHandler {
            public:
                ActionHandler();

                bool TryExecute(RequestInfo * request) override;
                void addHttpMethod(string method);

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