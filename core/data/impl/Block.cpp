#include <fstream>
#include <math.h>
#include <core/data/impl/BlockStorage.h>
#include <core/data/impl/Block.h>

using namespace std;
using namespace core::data::impl;

struct  Block::impl
{
	const size_t cachedHeaderSize = 5;

	size_t id;
	size_t sectorSize;
    BlockStorage * storage;
	char * firstSector;
	std::fstream * stream;
	size_t cachedHeaderValue[5] = { 0 };
	bool isFirstSectorDirty;

    impl()
	{
		firstSector = NULL;
		storage = NULL;
		isFirstSectorDirty = false;
	}

	~impl()
	{
		if (firstSector != NULL)
			delete firstSector;

		if (storage != NULL)
			delete storage;
	}
};

Block::Block(BlockStorage * storage, size_t id, char * firstSector, size_t sectorSize, fstream * stream) : pImpl(new Block::impl())
{
    pImpl->id = id;
	pImpl->storage = storage;
	pImpl->firstSector = firstSector;
	pImpl->sectorSize = sectorSize;
	pImpl->stream = stream;
};

Block::~Block (){
	this->Flush();
    pImpl->storage->Delete(this->Id());
};

inline size_t Block::Id() const {
    return pImpl->id;
};

size_t Block::GetHeader(size_t field) {

	if (field >= (pImpl->storage->BlockHeaderSize() / 8)) {
		ERROR_WRITE(STR("Invalid field : ") << field);
		return 0;
	}

	if (field < pImpl->cachedHeaderSize)
	{
		if (pImpl->cachedHeaderValue[field] == 0)
		{
			char buffer[8] = { '\0' };
			memcpy(buffer, pImpl->firstSector, field * 8);
			pImpl->cachedHeaderValue[field] = bitsToInt(buffer);
		}

		return (long)pImpl->cachedHeaderValue[field];
	}
	else
	{
		char buffer[8] = {0};
		memcpy(buffer, pImpl->firstSector, field * 8);
		pImpl->cachedHeaderValue[field] = bitsToInt(buffer);

		return bitsToInt(buffer);
	}
};

void Block::SetHeader(size_t field, size_t value){

	if (field < pImpl->cachedHeaderSize)
		pImpl->cachedHeaderValue[field] = value;

	char * buffer = intToBits(value); // Fixit : intToBits return sizeof(size_t) but field 8 byte

	memcpy(buffer, pImpl->firstSector, field * 8);

	memcpy(pImpl->firstSector, buffer, field * 8);
	pImpl->isFirstSectorDirty = true;
};

void Block::Write(char const * src, size_t srcLen, size_t dstOffset, size_t srcOffset, size_t count){
	if (dstOffset + count > pImpl->storage->BlockContentSize()) {
		ERROR_WRITE(STR("Count argument is outside of dest bounds: Count=" << count));
		return;
	}

	if (srcOffset + count > srcLen) {
		ERROR_WRITE(STR("Count argument is outside of src bounds: Count=" << count));
		return;
	}
	
	if ((pImpl->storage->BlockHeaderSize() + dstOffset) < pImpl->storage->DiskSectorSize()) {
		auto writeSize = std::min(count, pImpl->storage->DiskSectorSize() - pImpl->storage->BlockHeaderSize() - dstOffset);

		memcpy(pImpl->firstSector + pImpl->storage->BlockHeaderSize() + dstOffset, src + srcOffset, writeSize);
		pImpl->isFirstSectorDirty = true;
	}

	if ((pImpl->storage->BlockHeaderSize() + dstOffset + count) > pImpl->storage->DiskSectorSize())
	{
		pImpl->stream->seekg((long long int) ((pImpl->id * pImpl->storage->BlockSize()) + std::max(pImpl->storage->DiskSectorSize(), pImpl->storage->BlockHeaderSize() + dstOffset)), ios::beg);

		auto d = pImpl->storage->DiskSectorSize() - (pImpl->storage->BlockHeaderSize() + dstOffset);
		if (d > 0) {
			dstOffset += d;
			srcOffset += d;
			count -= d;
		}

		size_t written = 0L;
		while (written < count)
		{
			auto bytesToWrite = std::min((size_t)pImpl->storage->BlockSize(), count - written);
			pImpl->stream->seekg((long long int) (srcOffset + written), ios::cur);
			pImpl->stream->write((char*)src, bytesToWrite);
			pImpl->stream->flush();
			written += bytesToWrite;
		}
	}
};

void Block::Read(char * dst, size_t dstLen, size_t dstOffset, size_t srcOffset, size_t count){
    if (dstOffset + count > pImpl->storage->BlockContentSize() || count <= 0) {
        ERROR_WRITE(STR("Count argument is outside of dest bounds: Count=" << count));
        return;
    }

    if (dstOffset + count > dstLen) {
        ERROR_WRITE(STR("Count argument is outside of src bounds: Count=" << count));
        return;
    }

    auto dataCopied = 0;
    auto copyFromFirstSector = (pImpl->storage->BlockHeaderSize() + srcOffset) < pImpl->storage->DiskSectorSize();
    if (copyFromFirstSector)
    {
        auto copyWidth = std::min(pImpl->storage->DiskSectorSize() - pImpl->storage->BlockHeaderSize() - srcOffset, count);
        memcpy(dst + srcOffset, pImpl->firstSector + pImpl->storage->BlockHeaderSize() + dstOffset, copyWidth);
        dataCopied += copyWidth;
    }

    if (dataCopied < count) {
        if (copyFromFirstSector) {
            pImpl->stream->seekg((long long int) ((this->Id() * pImpl->storage->BlockSize()) + pImpl->storage->DiskSectorSize()), ios::beg);
        } else {
            pImpl->stream->seekg((long long int) ((this->Id() * pImpl->storage->BlockSize()) + pImpl->storage->BlockHeaderSize() + srcOffset), ios::beg);
        }
    }
}

void Block::Flush()
{
	if (pImpl->isFirstSectorDirty && pImpl->firstSector != NULL)
	{
		pImpl->stream->seekg((long long int) (pImpl->id * pImpl->storage->BlockSize()), ios::beg);
		pImpl->stream->write(pImpl->firstSector, pImpl->storage->BlockSize());
		pImpl->stream->flush();

		pImpl->isFirstSectorDirty = false;
	}
}
