#include <fstream>
#include <math.h>
#include <core/data/impl/BlockStorage.h>
#include <core/data/impl/Block.h>

using namespace std;
using namespace core::data::impl;


struct BlockStorage::BlockStorageImpl
{
    fstream * stream;
    size_t diskSectorSize;
    size_t blockSize;
    size_t blockHeaderSize;
    size_t blockContentSize;
    string filePath;
    CUSTOM_MAP<size_t, Block * > blocks;

    BlockStorageImpl()
    {
#ifdef USE_GOOGLE_DENSE_HASH_MAP
        INIT_MAP(blocks, -2, -1)
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
    pImpl->blockContentSize = blockSize - blockHeaderSize;

    pImpl->stream = new fstream();
    pImpl->stream->open(path, std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binary);
    pImpl->stream->flush();
}

BlockStorage::~BlockStorage()
{
    delete this->pImpl.release();
}

Block * Find(size_t id){
    return NULL;
};

size_t BlockStorage::BlockContentSize() const {
    return pImpl->blockContentSize;
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

Block *BlockStorage::Create() {

    std::ios::pos_type begin = pImpl->stream->tellg();
    pImpl->stream->seekg(0, std::ios::end);
    std::ios::pos_type end = pImpl->stream->tellg();
    auto fileLength = end - begin;

    if ((pImpl->stream->tellg() % pImpl->blockSize) != 0) {
        ERROR_WRITE(STR("Unexpected length of the stream: ") << pImpl->stream->tellg());
        return NULL;
    }

    auto blockId = (size_t)ceil ((double)pImpl->stream->tellg() / (double)pImpl->blockSize);

    // Extend length of underlying stream
    auto size = ((long)((blockId * pImpl->blockSize) + pImpl->blockSize)) - fileLength;

    for(unsigned long i=0;i<size;++i)
        pImpl->stream->put('\0');

    pImpl->stream->flush ();

    auto * block = new Block (*this, blockId, pImpl->diskSectorSize, pImpl->stream);
    pImpl->blocks[block->Id()] = block;
    return block;
};