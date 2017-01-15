#pragma once

#include <config.h>
#include <core/MimeType.h>
#include <core/server/ServerConfig.h>
#include <core/server/handler/StaticFileHandler.h>
#include <core/server/handler/BaseHandler.h>

namespace core {
    namespace server {
        namespace handler {
            class StaticFileHandler : public BaseHandler {
            public:
                StaticFileHandler();

                bool TryExecute(RequestInfo * request) override;

            private:
                CUSTOM_MAP<string, string> fileCache;
            };
        }
    }
}