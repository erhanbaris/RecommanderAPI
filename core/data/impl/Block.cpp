#include <fstream>
#include <math.h>
#include <core/data/impl/BlockStorage.h>
#include <core/data/impl/Block.h>

using namespace std;
using namespace core::data::impl;

struct  Block::BlockImpl
{
    size_t id;
};

Block::Block(BlockStorage & storage, size_t id, size_t firstSectorSize, fstream * stream) : pImpl(new BlockImpl())
{
    pImpl->id = id;
};

size_t Block::Id() const {
    return pImpl->id;
}