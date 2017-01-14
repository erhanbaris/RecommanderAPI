#pragma once

#include "../../../config.h"
#include "../../MimeType.h"

#include "../ServerConfig.h"
#include "StaticFileHandler.h"
#include "BaseHandler.h"

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