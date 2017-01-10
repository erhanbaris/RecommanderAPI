#include "BaseDataInfo.h"

using namespace std;
using namespace core;
using namespace core::algoritm;


core::data::BaseDataInfo::BaseDataInfo() {
    INT_INIT_MAP(productInfos);
    INT_INIT_MAP(userRatings);
    INT_INIT_MAP(recommendCacheForUser);

    INT_INIT_MAP(productMap);
    INT_INIT_MAP(userMap);
}

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
};