#pragma once
#include <config.h>
#include <core/server/ServerConfig.h>


namespace core {
    namespace server {
        namespace controller {
            typedef std::function<ResponseInfo(RequestInfo *)> FuncDelegate;

            class BaseController {
            public:
                virtual void BeforeRequest(RequestInfo *);
                virtual void EndRequest(RequestInfo *);

            protected:
                CUSTOM_MAP<string,FuncDelegate> methods;
            };
        }
    }
}
