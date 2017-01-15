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


    size_t counter = 0;
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

    in = new ifstream(this->productFilePath.c_str());
    if (!in->is_open())
    {
        delete in;
        LOG_WRITE("!!!! PRODUCT FILE NOT FOUND !!!!");
        LOG_WRITE("DATABASE LOADING ENDED");
        return;
    }

    while (in->good()) {
        try {
            string movieIdStr, title, genres;
            getline(*in, movieIdStr, ',');
            getline(*in, title, ',');
            getline(*in, genres, '\n');

            this->Data()->AddProduct((PRODUCT_TYPE) stoi(movieIdStr), core::getString(title));
        }
        catch (const std::exception &e) {
            ERROR_WRITE(e.what());
        }
    }

    in->close();
    delete in;

    this->Data()->Prepare();
    LOG_WRITE("DATABASE LOADING ENDED");
};

template class core::data::CvsDataSource<core::data::GeneralDataInfo>;
