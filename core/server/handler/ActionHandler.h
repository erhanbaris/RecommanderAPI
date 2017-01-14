#pragma once

#include <functional>

#include "../../../config.h"

#include "../ServerConfig.h"
#include "StaticFileHandler.h"
#include "BaseHandler.h"

namespace core {
    namespace server {
        namespace handler {
            typedef std::function<ResponseInfo(RequestInfo*)> FuncDelegate;
            typedef CUSTOM_MAP<string, FuncDelegate> HttpMethodRoute;

            class ActionHandler : public BaseHandler {
            public:
                ActionHandler();

                bool TryExecute(RequestInfo * request) override;
                void addHttpMethod(string method);

                void SetGetAction(string url, FuncDelegate delegate);
                void SetPostAction(string url, FuncDelegate delegate);
                void SetDeleteAction(string url, FuncDelegate delegate);
                void SetPutAction(string url, FuncDelegate delegate);
                void SetHeadAction(string url, FuncDelegate delegate);

            private:
                CUSTOM_MAP<string, HttpMethodRoute> routeTable;
            };
        }
    }
}