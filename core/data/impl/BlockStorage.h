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
            class Block;
            class BlockStorage
            {
            public:
                size_t BlockContentSize() const;
                size_t BlockHeaderSize() const;
                size_t BlockSize() const;
                size_t DiskSectorSize() const;

                BlockStorage(std::string const & path, size_t blockSize = 4096, size_t blockHeaderSize = 48);
                ~BlockStorage();
                Block * Find(size_t id);
                Block * Create();

            private:
                struct BlockStorageImpl;
                unique_ptr<BlockStorageImpl> pImpl;
            };
        }
    }
}
