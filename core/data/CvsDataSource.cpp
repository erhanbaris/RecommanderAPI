#include <core/data/CvsDataSource.h>


using namespace std;
using namespace core;

template<class T>
void core::data::CvsDataSource<T>::LoadData() {
    LOG_WRITE("DATABASE LOADING STARTED");

    ifstream * in = new ifstream(ratingFilePath.c_str());
    if (!in->is_open())
    {
        delete in;
        LOG_WRITE("!!!! RATING FILE NOT FOUND !!!!");
        LOG_WRITE("DATABASE LOADING ENDED");
        return;
    }

    while (in->good()) {
        try {
            string ratingStr, productIdStr, customerIdStr, timeStr;
            getline(*in, customerIdStr, ',');
            getline(*in, productIdStr, ',');
            getline(*in, ratingStr, ',');
            getline(*in, timeStr, '\n');

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

    wifstream * win = new wifstream(this->productFilePath.c_str());
    if (!win->is_open())
    {
        delete in;
        LOG_WRITE("!!!! PRODUCT FILE NOT FOUND !!!!");
        LOG_WRITE("DATABASE LOADING ENDED");
        return;
    }

    while (win->good()) {
        try {
            wchar_t LineOfChars[512];
            wchar_t movieIdStr[16], title[512], genres[512];
            (*win).getline(movieIdStr, 16, ',');
            (*win).getline(title, 512, ',');
            (*win).getline(genres, 512, '\r');


            this->Data()->AddProduct((PRODUCT_TYPE) stoi(movieIdStr), title);

            string tmpTitle = core::getNarrow(title);
            std::transform(tmpTitle.begin(), tmpTitle.end(), tmpTitle.begin(), ::tolower);

            string::iterator begin = tmpTitle.begin();
            string::iterator end = tmpTitle.end();
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
    LOG_WRITE("DATABASE LOADING ENDED");
};

template class core::data::CvsDataSource<core::data::GeneralDataInfo>;
