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
                void AddAlias(STR_TYPE alias, STR_TYPE fileName);

            private:
                CUSTOM_MAP<STR_TYPE, STR_TYPE> fileCache;
                CUSTOM_MAP<STR_TYPE, STR_TYPE> fileAliases;
            };
        }
    }
}