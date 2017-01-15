#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "../Utils.h"
#include "BaseDataSource.h"

using namespace std;
using namespace core;

namespace core
{
	namespace data {
		template<class T>
		class CvsDataSource : public BaseDataSource<T> {
		public:
			CvsDataSource(string pProductFilePath, string pRatingFilePath)
			{
				productFilePath = pProductFilePath;
				ratingFilePath = pRatingFilePath;
			}

			virtual void LoadData() override;

		private:
			string productFilePath;
			string ratingFilePath;
		};
	}
}