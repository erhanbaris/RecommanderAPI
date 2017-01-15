#pragma once

#include <core/server/action/BaseAction.h>

namespace core {
    namespace server {
        namespace action {
            class UserRecommendAction: public BaseAction {
            public:
                string Url() override;
                ResponseInfo Execute(RequestInfo *info) override;
                short MethodInfo() override;
                vector<pair<PRODUCT_TYPE, wstring>> recommend(USER_TYPE userId);
            };
        }
    }
}
