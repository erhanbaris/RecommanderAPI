#pragma once

#include <core/server/ServerConfig.h>

namespace core {
    namespace server {
        namespace handler {
            class BaseHandler {
            public:
                virtual bool TryExecute(RequestInfo * request) = 0;
            };
        }
    }
}