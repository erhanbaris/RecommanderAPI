#include <core/data/CvsDataSource.h>
#include <functional>

using namespace std;
using namespace core;

template<class T>
void core::data::CvsDataSource<T>::LoadData() {
    LOG_WRITE(STR("DATABASE LOADING STARTED"));

    IFSTREAM_TYPE * in = new IFSTREAM_TYPE(ratingFilePath.c_str());
    if (!in->is_open())
    {
        delete in;
        LOG_WRITE(STR("!!!! RATING FILE NOT FOUND !!!!"));
        LOG_WRITE(STR("DATABASE LOADING ENDED"));
        return;
    }

    while (in->good()) {
        try {
            STR_TYPE ratingStr, productIdStr, customerIdStr, timeStr;
			getline(*in, customerIdStr, STR(','));
            getline(*in, productIdStr, STR(','));
            getline(*in, ratingStr, STR(','));

            PRODUCT_TYPE productId = stoi(productIdStr);
            size_t customerId = stoi(customerIdStr);
            float rating = stof(ratingStr);
            char ratingChar = rating /** 10*/;

            this->Data()->AddUserRating(customerId, productId, ratingChar);
        }
        catch (const std::exception &e) {
            ERROR_WRITE(e.what());
        }
    }

    in->close();
    delete in;

    IFSTREAM_TYPE * win = new IFSTREAM_TYPE(this->productFilePath.c_str());
    if (!win->is_open())
    {
        delete win;
        LOG_WRITE(STR("!!!! PRODUCT FILE NOT FOUND !!!!"));
        LOG_WRITE(STR("DATABASE LOADING ENDED"));
        return;
    }

    auto hashFunc = std::hash<STR_TYPE>();

    while (win->good()) {
        try {
            STR_TYPE movieIdStr, title, genres;
            getline(*win, movieIdStr, STR(','));
            getline(*win, title, STR(','));
            getline(*win, genres, STR(','));
            getline(*win, genres, STR('\n'));

            auto movieId = hashFunc(movieIdStr);
            this->Data()->AddProduct((PRODUCT_TYPE) movieId, title);

            std::transform(title.begin(), title.end(), title.begin(), ::tolower);

			STR_TYPE::iterator end = title.end();
            for (auto current = title.begin(); current != end; ++current) {
                
                if (((*current) >= 'A' && (*current) <= 'Z') ||
                    ((*current) >= 'a' && (*current) <= 'z') ||
                    ((*current) >= '0' && (*current) <= '9'))
                    continue;
                
                (*current) = ' ';
            }

            auto parts = core::splitString(title, ' ');
            auto partsEnd = parts.end();

            for (auto part = parts.begin(); part != partsEnd ; ++part) {
                if (part->size() > 1)
                    this->Data()->symspell.CreateDictionaryEntry(*part, movieId);
            }

            //this->Data()->symspell.CreateDictionaryEntry(title, movieId);
        }
        catch (const std::exception &e) {
            ERROR_WRITE(e.what());
        }
    }

    win->close();
    delete win;

    this->Data()->Prepare();
    LOG_WRITE(STR("DATABASE LOADING ENDED"));
};

template class core::data::CvsDataSource<core::data::GeneralDataInfo>;
