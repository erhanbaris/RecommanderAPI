#pragma once
#include <fstream>
#include <config.h>

#include <iostream>
#include <vector>

using namespace std;

namespace core {

    namespace data {
        namespace impl {
            class BlockStorage;
            class Block
            {
            public:
                size_t Id() const;

                Block (BlockStorage * storage, size_t id, size_t firstSectorSize, fstream * stream);
                ~Block ();
                void Read(uint8_t * dst, size_t dstOffset, size_t srcOffset, size_t count);
                void Write (uint8_t * src, size_t srcOffset, size_t dstOffset, size_t count);

            private:
                struct BlockImpl;
                unique_ptr<BlockImpl> pImpl;
            };
        }
    }
}
