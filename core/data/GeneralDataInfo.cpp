#include <core/data/GeneralDataInfo.h>

using namespace std;
using namespace core;
using namespace core::algoritm;

core::data::GeneralDataInfo::GeneralDataInfo() {
    productMapEnd = productMap.end();
    userMapEnd = userMap.end();
}

void core::data::GeneralDataInfo::AddUserRating(USER_TYPE customerId, PRODUCT_TYPE productId, RATE_TYPE rating) {
    try {
        IndexItem *indexItem = new IndexItem();
        indexItem->ProductId = productId;
        indexItem->UserId = customerId;
        indexItem->Rating = rating;

        if (productMap.find(productId) == productMapEnd) {
            this->productMap[productId] = std::vector<IndexItem *>();
            this->productMapEnd = this->productMap.end();
        }

        if (userMap.find(customerId) == userMapEnd) {
            this->userMap[customerId] = UserInfo();
            this->userMapEnd = userMap.end();

            ++core::TotalUserCount;
        }

        auto &userMapCache = userMap[customerId];
        productMap[productId].push_back(indexItem);
        userMapCache.Products.push_back(indexItem);

        if (userRatings.find(customerId) == userRatings.end())
            userRatings[customerId] = vector<model::Rating>();

        userMapCache.TotalRating += rating;
        ++userMapCache.TotalProduct;
        userMapCache.AvgRating += userMapCache.TotalRating / userMapCache.TotalProduct;

        core::TotalProductCount++;
        core::TotalRating += rating;

        userRatings[customerId].push_back(model::Rating(productId, rating));
    }
    catch (const std::exception &e) {
        ERROR_WRITE(e.what());
    }
}

void core::data::GeneralDataInfo::AddProduct(PRODUCT_TYPE movieId, wstring title) {
    productInfos[movieId] = title;
};

void core::data::GeneralDataInfo::Prepare() {
    core::AvgProductCount = (core::TotalProductCount / core::TotalUserCount);
    core::AvgRating = core::TotalRating / core::TotalProductCount;

    LOG_WRITE("Slope One Init Started");
    slopeOne.AddUsers(userMap);
    LOG_WRITE("Slope One Init Finished");
};

/*
template<class DataInfo, class DataSource>
class RecommandExecuter {
public:
    virtual void SetDataInfo(shared_ptr<DataInfo> pInfo) {
        this->info = pInfo;
    }

    virtual void SetDataSource(shared_ptr<DataSource> pSource) {
        this->source = pSource;
    }

    shared_ptr<DataInfo> GetDataInfo(shared_ptr<DataInfo> pInfo) {
        return this->info;
    }

    shared_ptr<DataSource> GetDataSource() {
        return this->source;
    }

private:
    shared_ptr<DataInfo> info;
    shared_ptr<DataSource> source;
};*/