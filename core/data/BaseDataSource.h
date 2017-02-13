#pragma once

#include <config.h>
#include <model/Rating.h>
#include <core/data/BaseDataInfo.h>
#include <core/data/GeneralDataInfo.h>

using namespace model;
using namespace core;

namespace core {
	namespace data {
		
		template<class DataInfo = GeneralDataInfo>
		class BaseDataSource {
		public:
			static_assert(std::is_base_of<BaseDataInfo, DataInfo>::value, "'BaseDataInfo' Required");

			BaseDataSource()
			{
				data = new DataInfo();
			}

			BaseDataInfo * Data() const
			{
				return data;
			}

			virtual ~BaseDataSource() = default;
			virtual void LoadData() = 0;

		private:
			BaseDataInfo * data;
		};
	}
}
