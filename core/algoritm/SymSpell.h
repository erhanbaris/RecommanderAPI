#pragma once

#define ENABLE_TEST

#undef ENABLE_TEST

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <fstream>
#include <list>
#include <config.h>
#include <core/Utils.h>

using namespace std;

namespace core {
    namespace algoritm {
    
        class dictionaryItemContainer {
        public:
            dictionaryItemContainer() {
                Id = new CUSTOM_SET<PRODUCT_TYPE>();
                Id->resize(64);
                INIT_SET(*Id, 0, -1);
            }

            CUSTOM_SET<PRODUCT_TYPE> * Id;
            vector<size_t> suggestions;
            size_t count = 0;
        };

        class FindedItem {
        public:
            unsigned short distance = 0;
            PRODUCT_TYPE productId;
        };

        class SymSpell {
        public:
            SymSpell();

            bool CreateDictionaryEntry(STR_TYPE key, PRODUCT_TYPE id);
            vector<pair<PRODUCT_TYPE, unsigned short> > Find(STR_TYPE input) const;
            void Info();
            void SaveIndex();

        private:
            void AddLowestDistance(dictionaryItemContainer * item, STR_TYPE suggestion, size_t suggestionint, STR_TYPE del);
            void Edits(STR_TYPE word, CUSTOM_SET<STR_TYPE> &deletes) const;
            vector<pair<PRODUCT_TYPE, FindedItem> >* Lookup(STR_TYPE input, size_t editDistanceMax) const;
            static size_t DamerauLevenshteinDistance(const STR_TYPE &s1, const STR_TYPE &s2);
        };
    }
}
