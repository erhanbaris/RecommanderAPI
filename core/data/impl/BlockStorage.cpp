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
	CUSTOM_MAP<size_t, Block * >::iterator blocksEnd;

    BlockStorageImpl()
    {
#ifdef USE_GOOGLE_DENSE_HASH_MAP
		INIT_MAP(blocks, -2, -1);
#endif
		blocksEnd = blocks.end();
    }

    ~BlockStorageImpl()
    {
        if (stream != NULL)
        {
            stream->close();
            delete stream;
        }
    }

	size_t getFileSize() const
	{
		this->stream->seekg(0, std::ios::beg);
		std::ios::pos_type begin = this->stream->tellg();
		this->stream->seekg(0, std::ios::end);
		std::ios::pos_type end = this->stream->tellg();
		return (size_t) (end - begin);
	}
};

BlockStorage::BlockStorage(std::string const &path, size_t blockSize, size_t blockHeaderSize) : pImpl(new BlockStorageImpl())  {
    pImpl->filePath = path;
    pImpl->blockSize = blockSize;
    pImpl->blockHeaderSize = blockHeaderSize;
    pImpl->diskSectorSize = ((blockSize >= 4096) ? 4096 : 128);
    pImpl->blockContentSize = blockSize - blockHeaderSize;

    pImpl->stream = new fstream();
    pImpl->stream->open(pImpl->filePath, ios::in | ios::out | ios::binary);
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
    if ((pImpl->getFileSize() % pImpl->blockSize) != 0) {
        ERROR_WRITE(STR("Unexpected length of the stream: ") << pImpl->stream->tellg());
        return NULL;
    }

    auto blockId = (size_t)ceil ((double)pImpl->getFileSize() / (double)pImpl->blockSize);

    // Extend length of underlying stream
    auto size = ((long)((blockId * pImpl->blockSize) + pImpl->blockSize)) - pImpl->getFileSize();

    for(unsigned long i = 0; i < size; ++i)
        pImpl->stream->put('\0');

    pImpl->stream->flush ();

	auto * diskSector = new char[pImpl->diskSectorSize];
    auto * block = new Block (this, blockId, diskSector, pImpl->diskSectorSize, pImpl->stream);
    pImpl->blocks[block->Id()] = block;
	pImpl->blocksEnd = pImpl->blocks.end();

    return block;
};

Block * BlockStorage::Find(size_t id)
{
	if (pImpl->blocksEnd != pImpl->blocks.find(id))
		return pImpl->blocks[id];

	auto blockPosition = id * pImpl->blockSize;
	if ((blockPosition + pImpl->blockSize) > pImpl->getFileSize())
		return NULL;

	auto * firstSector = new char[pImpl->blockSize];
	pImpl->stream->seekg((long long int) (id * pImpl->blockSize), ios::beg);
	pImpl->stream->read((char *)firstSector, pImpl->diskSectorSize);

	auto * block = new Block(this, id, firstSector, pImpl->diskSectorSize, pImpl->stream);
	return block;
};

void BlockStorage::Delete(size_t id) {
    pImpl->blocks.erase(id);
	pImpl->blocksEnd = pImpl->blocks.end();
};
