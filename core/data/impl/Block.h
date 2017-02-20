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

                Block (BlockStorage * storage, size_t id, char * firstSector, size_t sectorSize, std::fstream * stream);
                ~Block ();
                void Read(char * dst, size_t & srcLen, size_t dstOffset, size_t srcOffset, size_t count);
                void Write (char const * src, size_t srcLen, size_t srcOffset, size_t dstOffset, size_t count);
				void Flush();
				size_t GetHeader(size_t field);
				void SetHeader(size_t field, size_t value);

            private:
                struct BlockImpl;
                unique_ptr<BlockImpl> pImpl;
            };
        }
    }
}
