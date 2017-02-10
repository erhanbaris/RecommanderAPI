#include <core/data/CvsDataSource.h>


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
			in->getline(customerIdStr.c_str(), 16, L',');
            in->getline(productIdStr, 16, L',');
            in->getline(ratingStr, 16, L',');

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
        delete in;
        LOG_WRITE(STR("!!!! PRODUCT FILE NOT FOUND !!!!"));
        LOG_WRITE(STR("DATABASE LOADING ENDED"));
        return;
    }

    while (win->good()) {
        try {
            wchar_t LineOfChars[512];
            wchar_t movieIdStr[16], title[512], genres[512];
            win->getline(movieIdStr, 16, ',');
            win->getline(title, 512, ',');
            win->getline(genres, 512, '\r');


            this->Data()->AddProduct((PRODUCT_TYPE) stoi(movieIdStr), title);

			STR_TYPE tmpTitle = core::getNarrow(title);
            std::transform(tmpTitle.begin(), tmpTitle.end(), tmpTitle.begin(), ::tolower);

            STR_TYPE::iterator begin = tmpTitle.begin();
			STR_TYPE::iterator end = tmpTitle.end();
            for (auto current = tmpTitle.begin(); current != end; ++current) {
                
                if (((*current) >= 'A' && (*current) <= 'Z') ||
                    ((*current) >= 'a' && (*current) <= 'z') ||
                    ((*current) >= '0' && (*current) <= '9'))
                    continue;
                
                (*current) = ' ';
            }

            auto parts = core::splitString(tmpTitle, ' ');
            auto partsEnd = parts.end();

            for (auto part = parts.begin(); part != partsEnd ; ++part) {
                if (part->size() > 1)
                    this->Data()->symspell.CreateDictionaryEntry(*part, stoi(movieIdStr));
            }

            this->Data()->symspell.CreateDictionaryEntry(tmpTitle, stoi(movieIdStr));
            this->Data()->symspell.CreateDictionaryEntry(core::getString(title), stoi(movieIdStr));
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
