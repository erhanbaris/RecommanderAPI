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
			CvsDataSource(STR_TYPE pProductFilePath, STR_TYPE pRatingFilePath)
			{
				productFilePath = pProductFilePath;
				ratingFilePath = pRatingFilePath;
			}

			virtual void LoadData() override;

		private:
			STR_TYPE productFilePath;
			STR_TYPE ratingFilePath;
		};
	}
}