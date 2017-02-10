#pragma once

#include <core/server/action/BaseAction.h>

namespace core {
    namespace server {
        namespace action {
            class UserRecommendAction: public BaseAction {
            public:
                STR_TYPE Url() override;
                ResponseInfo Execute(RequestInfo *info) override;
                short MethodInfo() override;
                BaseAction* CreateObject() override;
                vector<pair<PRODUCT_TYPE, STR_TYPE>> recommend(USER_TYPE userId);
            };
        }
    }
}
