#pragma once

#include <config.h>
#include <core/server/ServerConfig.h>
#include <core/Utils.h>

#define TYPE_GET 1u << 0
#define TYPE_POST 1u << 1
#define TYPE_PUT 1u << 2
#define TYPE_DEL 1u << 4
#define TYPE_HEAD 1u << 8

namespace core {
    namespace server {
        namespace action {

            class BaseAction {
            public:
                virtual string Url() = 0;
                virtual ResponseInfo Execute(RequestInfo *info) = 0;
                virtual short MethodInfo() = 0;
                virtual BaseAction* CreateObject() = 0;

            protected:
                std::string EscapeJsonString(const std::string &input);
            };
        }
    }
}
