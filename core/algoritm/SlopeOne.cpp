#include <core/algoritm/SlopeOne.h>

namespace {
#define getHashCode(item1Id, item2Id)\
     std::hash<double>{}(item1Id < item2Id ? (item1Id * 1000000) + item2Id : (item2Id * 1000000) + item1Id)

    typedef struct pRating {
        pRating() : Value(0), Freq(0) { }

        pRating(RATE_TYPE value, size_t freq) : Value(value), Freq(freq) { }

        RATE_TYPE Value;
        size_t Freq;

        float AverageValue() const {
            if (Value == 0 || Freq == 0)
                return 0.0;

            return static_cast<float>(Value) / static_cast<float>(Freq);
        }

    } rating;

    CUSTOM_MAP<size_t, rating *> DifffMatrix;
    CUSTOM_SET<PRODUCT_TYPE> Items;

    bool isKeyContain(PRODUCT_TYPE item1Id, PRODUCT_TYPE item2Id) {
        return DifffMatrix.find(getHashCode(item1Id, item2Id)) != DifffMatrix.end();
    }

    rating *getValue(PRODUCT_TYPE item1Id, PRODUCT_TYPE item2Id) {
        return DifffMatrix[getHashCode(item1Id, item2Id)];
    }

    void setValue(PRODUCT_TYPE item1Id, PRODUCT_TYPE item2Id, rating *item) {
        DifffMatrix.insert(std::pair<PRODUCT_TYPE, rating *>(getHashCode(item1Id, item2Id), item));
    }
}

core::algoritm::SlopeOne::SlopeOne() {
#ifdef USE_GOOGLE_DENSE_HASH_MAP
    DifffMatrix.set_deleted_key(-1);
    DifffMatrix.set_empty_key(0);

    Items.set_deleted_key(-1);
    Items.set_empty_key(0);

    DifffMatrix.resize(RESERVED_SIZE);
    Items.resize(RESERVED_SIZE);
#endif
}

void core::algoritm::SlopeOne::AddUsers(CUSTOM_MAP<USER_TYPE, core::UserInfo> &users) {
    auto usersEnd = users.end();
    for (auto item = users.begin(); item != usersEnd; ++item)
        AddUserRatings(item->second);
}

void core::algoritm::SlopeOne::AddUserRatings(core::UserInfo &userRatings) {
    auto userRatingsEnd = userRatings.Products.end();
    for (auto item1 = userRatings.Products.begin(); item1 != userRatingsEnd; ++item1) {
        PRODUCT_TYPE item1Id = (*item1)->ProductId;
        RATE_TYPE item1Rating = (*item1)->Rating;
        Items.insert((*item1)->ProductId);

        for (auto item2 = userRatings.Products.begin(); item2 != userRatingsEnd; ++item2) {
            if ((*item1)->ProductId <= item1Id)
                continue;

            PRODUCT_TYPE item2Id = (*item1)->ProductId;
            RATE_TYPE item2Rating = (*item1)->Rating;

            rating *ratingDiff;
            if (isKeyContain(item1Id, item2Id))
                ratingDiff = getValue(item1Id, item2Id);
            else {
                ratingDiff = new rating;
                setValue(item1Id, item2Id, ratingDiff);
            }

            ratingDiff->Value += item1Rating - item2Rating;
            ++ratingDiff->Freq;
        }
    }
}

vector<pair<PRODUCT_TYPE, float>> core::algoritm::SlopeOne::Predict(CUSTOM_MAP<PRODUCT_TYPE, RATE_TYPE> &userRatings) {
    vector<pair<PRODUCT_TYPE, float>> returnValue;
    auto DifffMatrixEnd = DifffMatrix.end();
    auto ItemsEnd = Items.end();
    auto userRatingsEnd = userRatings.end();

    for (auto itemId = Items.begin(); itemId != ItemsEnd; ++itemId) {
        auto item = *itemId;

        if (userRatings.find(item) != userRatingsEnd)
            continue;

        rating itemRating;

        for (auto userRating = userRatings.begin(); userRating != userRatingsEnd; ++userRating) {
            if (userRating->first == item)
                continue;

            USER_TYPE inputItemId = userRating->first;
            if (DifffMatrix.find(getHashCode(item, inputItemId)) != DifffMatrixEnd) {
                auto diff = DifffMatrix[getHashCode(item, inputItemId)];
                itemRating.Value +=
                        diff->Freq * (userRating->second + diff->AverageValue() * (item < userRating->first ? 1 : -1));
                itemRating.Freq += diff->Freq;
            }
        }

        returnValue.push_back(std::pair<PRODUCT_TYPE, float>(item, itemRating.AverageValue()));
    }

    std::sort(returnValue.begin(), returnValue.end(),
              [](pair<PRODUCT_TYPE, float> const &left, pair<PRODUCT_TYPE, float> const &right) {
                  return left.second > right.second;
              });

    return returnValue;
}
