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
            }

            ItemType itemType;
            size_t intValue;
            std::shared_ptr<dictionaryItem> dictValue;

#ifdef IO_OPERATIONS
            MSGPACK_DEFINE(itemType, intValue, dictValue);
#endif
        };

        class suggestItem {
        public:
            string term;
            unsigned short distance = 0;
            unsigned short count;

            bool operator==(const suggestItem &item) const {
                return term.compare(item.term) == 0;
            }

            size_t HastCode() const {
                return hash<string>()(term);
            }

#ifdef IO_OPERATIONS
            MSGPACK_DEFINE(term, distance, count);
#endif
        };

        class SymSpell {
        public:
            size_t Verbose = 0;
            size_t MaxDistance = 2;

            SymSpell();

#ifdef IO_OPERATIONS
            void Save(string filePath);
            void Load(string filePath);
#endif

            bool CreateDictionaryEntry(string key);
            vector<suggestItem> Correct(string input);

        private:
            size_t maxlength = 1000;
            CUSTOM_MAP<size_t, dictionaryItemContainer> dictionary;
            vector<string> wordlist;
            vector<string> parseWords(string text) const;
            void AddLowestDistance(shared_ptr<dictionaryItem> const &item, string suggestion, size_t suggestionint, string del);
            void Edits(string word, CUSTOM_SET<string> &deletes) const;
            vector<suggestItem> Lookup(string input, size_t editDistanceMax);
            static size_t DamerauLevenshteinDistance(const std::string &s1, const std::string &s2);
        };
    }
}