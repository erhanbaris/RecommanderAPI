#pragma once

#include <config.h>

#include <iostream>
#include <vector>

using namespace std;

namespace core {

    namespace data {
        namespace impl {
            class Block
            {
            public:
                PRODUCT_TYPE Id;
                void Read(uint8_t * dst, size_t dstOffset, size_t srcOffset, size_t count);
                void Write (uint8_t * src, size_t srcOffset, size_t dstOffset, size_t count);
            };
        }
    }
}
