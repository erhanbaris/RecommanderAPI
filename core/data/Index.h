#pragma once

#include <config.h>
#include <core/Utils.h>
#include <core/algoritm/SlopeOne.h>
#include <core/algoritm/SymSpell.h>
#include <model/Rating.h>

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace core;
using namespace core::algoritm;

namespace core {
    namespace data {
        class Index {
        public:
            Index();
			virtual ~Index();
            void SetFileName(string fileName);
			void SetDictionary(CUSTOM_MAP<size_t, dictionaryItemContainer> * dict);
            bool Open();
			bool Close();
            bool Create();
			bool IsOpen();

        private:
            string fileName;
			fstream* file;
			CUSTOM_MAP<size_t, dictionaryItemContainer>* dictionary;
        };
    }
}