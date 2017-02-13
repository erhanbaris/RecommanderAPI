#pragma once

#define ENABLE_TEST
#define IO_OPERATIONS

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

#undef IO_OPERATIONS
#ifdef IO_OPERATIONS
#include <msgpack.hpp>
#endif

using namespace std;

namespace core {
    namespace algoritm {
        class dictionaryItem {
        public:
            vector<size_t> suggestions;
            size_t count = 0;

            dictionaryItem(size_t c) {
                count = c;
            }

            dictionaryItem() {
                count = 0;
            }

            ~dictionaryItem() {
                suggestions.clear();
            }

#ifdef IO_OPERATIONS
            MSGPACK_DEFINE(suggestions, count);
#endif
        };

        enum ItemType {
            NONE, DICT, INTEGER
        };

#ifdef IO_OPERATIONS
        MSGPACK_ADD_ENUM(ItemType);
#endif

        class dictionaryItemContainer {
        public:
            dictionaryItemContainer() : itemType(NONE), intValue(0) {
                Id = new CUSTOM_SET<PRODUCT_TYPE>();
                INIT_SET(*Id, 0, -1);
            }
            
            CUSTOM_SET<PRODUCT_TYPE> * Id;
            ItemType itemType;
            size_t intValue;
            std::shared_ptr<dictionaryItem> dictValue;

#ifdef IO_OPERATIONS
            MSGPACK_DEFINE(itemType, intValue, dictValue);
#endif
        };

        class FindedItem {
        public:
            unsigned short distance = 0;
            PRODUCT_TYPE productId;
        };

        class SymSpell {
        public:
            SymSpell();

#ifdef IO_OPERATIONS
            void Save(string filePath);
            void Load(string filePath);
#endif

            bool CreateDictionaryEntry(STR_TYPE key, PRODUCT_TYPE id);
            vector<pair<PRODUCT_TYPE, unsigned short> > Find(STR_TYPE input) const;

        private:
            void AddLowestDistance(shared_ptr<dictionaryItem> const &item, STR_TYPE suggestion, size_t suggestionint, STR_TYPE del);
            void Edits(STR_TYPE word, CUSTOM_SET<STR_TYPE> &deletes) const;
            CUSTOM_MAP<PRODUCT_TYPE, FindedItem>* Lookup(STR_TYPE input, size_t editDistanceMax) const;
            static size_t DamerauLevenshteinDistance(const STR_TYPE &s1, const STR_TYPE &s2);
        };
    }
}
