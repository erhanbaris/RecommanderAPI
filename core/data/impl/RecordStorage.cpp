#include <fstream>
#include <math.h>
#include <core/data/impl/RecordStorage.h>

using namespace std;
using namespace core::data::impl;


struct RecordStorage::impl
{
    BlockStorage * storage;

    impl()
    {
    }

    ~impl()
    {
    }
};

RecordStorage::RecordStorage(BlockStorage * blockStorage) : pImpl(new RecordStorage::impl())
{
    pImpl->storage = blockStorage;
}