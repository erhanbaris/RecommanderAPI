#pragma once

#include <config.h>
#include <core/Utils.h>
#include <core/data/impl/Block.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace core;
using namespace core::data::impl;

namespace core {

    namespace data {
        namespace impl {
            class BlockStorage
            {
            public:
                size_t BlockContentSize() const;
                size_t BlockHeaderSize() const;
                size_t BlockSize() const;
                size_t DiskSectorSize() const;

                BlockStorage(std::string const & path, size_t blockSize = 40960, size_t blockHeaderSize = 48);
                Block * Find(PRODUCT_TYPE id);
                Block * Create();

            private:
                struct BlockStorageImpl;
                unique_ptr<BlockStorageImpl> pImpl;
            };
        }
    }
}
