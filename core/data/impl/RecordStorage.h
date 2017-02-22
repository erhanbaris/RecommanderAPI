#pragma once

#include <config.h>
#include <core/Utils.h>

#include <iostream>
#include <vector>
#include <core/data/impl/BlockStorage.h>
#include <core/data/impl/Block.h>

using namespace std;
using namespace core;

namespace core {

    namespace data {
        namespace impl {
			struct tRecordInfo {
				size_t Length; 
				char* Data;
				tRecordInfo() : Data(NULL), Length(0){ }
			};

            class RecordStorage
            {
            public:

                RecordStorage(BlockStorage * blockStorage);
                void Update (size_t recordId, char* data, size_t dataLen);
				tRecordInfo Find (size_t recordId);
				size_t Create ();
				size_t Create (char* data, size_t dataLen);
                void Delete (size_t recordId);

            private:
                struct impl;
                unique_ptr<impl> pImpl;
            };
        }
    }
}
