#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <core/Utils.h>
#include <core/data/BaseDataSource.h>

using namespace std;
using namespace core;

namespace core
{
	namespace data {
		template<class T>
		class CvsDataSource : public BaseDataSource<T> {
		public:
            CvsDataSource(std::string pProductFilePath, std::string pRatingFilePath)
			{
				productFilePath = pProductFilePath;
				ratingFilePath = pRatingFilePath;
			}

			virtual void LoadData() override;

		private:
            std::string productFilePath;
            std::string ratingFilePath;
		};
	}
}
