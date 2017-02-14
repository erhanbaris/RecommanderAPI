#pragma once

#include <config.h>
#include <core/Utils.h>
#include <core/algoritm/SlopeOne.h>
#include <core/algoritm/SymSpell.h>
#include <model/Rating.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace core;
using namespace core::algoritm;

namespace core {
    namespace data {
        class Index {
        public:
            Index();
            void SetFileName(string fileName);
            bool Open();
            bool Create();

        private:
            string fileName;
        };
    }
}