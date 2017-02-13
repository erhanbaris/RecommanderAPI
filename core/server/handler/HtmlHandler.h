#pragma once

#include <config.h>

#include <core/server/ServerConfig.h>
#include <core/server/handler/BaseHandler.h>

namespace core {
    namespace server {
        namespace handler {
            class HtmlHandler : public BaseHandler {
            public:
                HtmlHandler();

                bool TryExecute(RequestInfo * request) override;
                void AddAlias(string alias, string fileName);

            private:
                CUSTOM_MAP<string, string> fileCache;
                CUSTOM_MAP<string, string> fileAliases;
            };
        }
    }
}