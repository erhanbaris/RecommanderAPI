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
                void Read(char * dst, size_t dstLen, size_t dstOffset, size_t srcOffset, size_t count);
                void Write (char const * src, size_t srcLen, size_t srcOffset, size_t dstOffset, size_t count);
				void Flush();
                long int GetHeader(short field);
				void SetHeader(short field, long int value);

            private:
                struct impl;
                unique_ptr<impl> pImpl;
            };
        }
    }
}
