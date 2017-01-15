#pragma once

#include "../ServerConfig.h"

#include "../../../config.h"
#include "../../Utils.h"

namespace core {
    namespace server {
        namespace action {
            class BaseAction {
            public:
                virtual string Url() = 0;
                virtual ResponseInfo Execute(RequestInfo *info) = 0;
                std::string escapeJsonString(const std::string &input);
            };
        }
    }
}