#include "CvsDataSource.h"


using namespace std;
using namespace core;

template<class T>
void core::data::CvsDataSource<T>::LoadData() {
    cout << "DATABASE LOADING STARTED" << endl;

    ifstream in(ratingFilePath);
    if (!in.is_open())
        return;

    cout << "-Rating Source Parsing Started" << endl;

    size_t counter = 0;
    while (in.good()) {
        try {
            string ratingStr, productIdStr, customerIdStr, timeStr;
            getline(in, customerIdStr, ',');
            getline(in, productIdStr, ',');
            getline(in, ratingStr, ',');
            getline(in, timeStr, '\n');

            PRODUCT_TYPE productId = stoi(productIdStr);
            size_t customerId = stoi(customerIdStr);
            float rating = stof(ratingStr);
            char ratingChar = rating /** 10*/;

            this->Data()->AddUserRating(customerId, productId, ratingChar);
#if _DEBUG
            if (counter >= 250000)
                break;
            ++counter;
#endif
        }
        catch (const std::exception &e) {
            cout << "Rating : " << e.what() << endl;
        }
    }

    in.close();
    cout << "-Rating Source Parsing Finished" << endl;

    cout << "-Product Source Parsing Started" << endl;

    in = ifstream(this->productFilePath.c_str());
    if (!in.is_open())
        return;


    if (in.good()) {
        string tmp;
        getline(in, tmp, ',');
        getline(in, tmp, ',');
        getline(in, tmp, '\n');
    }

    while (in.good()) {
        try {
            string movieIdStr, title, genres;
            getline(in, movieIdStr, ',');
            getline(in, title, ',');
            getline(in, genres, '\n');

            this->Data()->AddProduct((PRODUCT_TYPE) stoi(movieIdStr), core::getString(title));
        }
        catch (const std::exception &e) {
            cout << "Product : " << e.what() << endl;
        }
    }

    in.close();
    cout << "-Product Source Parsing Finished" << endl;

    this->Data()->Prepare();
    cout << "DATABASE LOADING ENDED" << endl;
};

template class core::data::CvsDataSource<core::data::GeneralDataInfo>;
