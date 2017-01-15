#pragma once

#include "../../config.h"
#include "../Utils.h"
#include "../algoritm/SlopeOne.h"
#include "../../model/Rating.h"
#include "BaseDataInfo.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace core;
using namespace core::algoritm;

namespace core {
    namespace data {
        class GeneralDataInfo : public BaseDataInfo {
        public:
            GeneralDataInfo();
            void AddUserRating(USER_TYPE customerId, PRODUCT_TYPE productId, RATE_TYPE rating) override;
            void AddProduct(PRODUCT_TYPE movieId, wstring title) override;
            void Prepare() override;

        protected:
            CUSTOM_MAP<PRODUCT_TYPE, vector<IndexItem*>>::iterator productMapEnd;
            CUSTOM_MAP<USER_TYPE, UserInfo>::iterator  userMapEnd;
        };
    }
}
