#pragma once

#include <config.h>
#include <core/Utils.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace core;

namespace core {

    namespace data {
        namespace impl {
            class RecordStorage
            {
            public:
                void Update (size_t recordId, char* data, size_t dataLen);
                std::tuple<size_t, char*> Find (size_t recordId);
                uint Create ();
                uint Create (char* data, size_t dataLen);
                void Delete (size_t recordId);

            private:
                struct impl;
                unique_ptr<impl> pImpl;
            };
        }
    }
}
