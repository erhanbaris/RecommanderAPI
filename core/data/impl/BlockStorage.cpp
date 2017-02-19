#include <fstream>
#include <core/data/impl/BlockStorage.h>


using namespace std;
using namespace core::data::impl;

struct BlockStorage::BlockStorageImpl
{
    ifstream * stream;
    size_t diskSectorSize;
    size_t blockSize;
    size_t blockHeaderSize;
    string filePath;
    CUSTOM_MAP<size_t, Block * > blocks;

    BlockStorageImpl()
    {
#ifdef USE_GOOGLE_DENSE_HASH_MAP
        INT_INIT_MAP((blocks));
#endif
    }

    ~BlockStorageImpl()
    {
        if (stream != NULL)
        {
            stream->close();
            delete stream;
        }
    }
};

BlockStorage::BlockStorage(std::string const &path, size_t blockSize, size_t blockHeaderSize) : pImpl(new BlockStorageImpl())  {
    pImpl->filePath = path;
    pImpl->blockSize = blockSize;
    pImpl->blockHeaderSize = blockHeaderSize;
    pImpl->diskSectorSize = ((blockSize >= 4096) ? 4096 : 128);

    pImpl->stream = new ifstream();
    pImpl->stream->open(path, ios::binary);
}

size_t BlockStorage::BlockContentSize() const {
    return pImpl->blockHeaderSize;
};

size_t BlockStorage::BlockHeaderSize() const{
    return pImpl->blockHeaderSize;
};

size_t BlockStorage::BlockSize()const {
    return pImpl->blockSize;
};

size_t BlockStorage::DiskSectorSize() const{
    return pImpl->diskSectorSize;
};