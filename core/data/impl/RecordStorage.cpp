#include <fstream>
#include <math.h>
#include <core/data/impl/RecordStorage.h>

using namespace std;
using namespace core::data::impl;


struct RecordStorage::impl
{
	const size_t MaxRecordSize = 4194304; // 4MB
	const size_t kNextBlockId = 0;
	const size_t kRecordLength = 1;
	const size_t kBlockContentLength = 2;
	const size_t kPreviousBlockId = 3;
	const size_t kIsDeleted = 4;

    BlockStorage * storage;

    impl() : storage(NULL) { }
    ~impl() { }
};

RecordStorage::RecordStorage(BlockStorage * blockStorage) : pImpl(new RecordStorage::impl())
{
    pImpl->storage = blockStorage;
}

tRecordInfo RecordStorage::Find(size_t recordId) {
	tRecordInfo returnValue;
	auto * block = pImpl->storage->Find(recordId);
	if (block != NULL)
	{
		if (block->GetHeader(pImpl->kIsDeleted) == 1)
			return returnValue;
		
		if (block->GetHeader(pImpl->kPreviousBlockId) != 0)
			return returnValue;

		returnValue.Length = block->GetHeader(pImpl->kRecordLength);

		if (returnValue.Length > pImpl->MaxRecordSize)
		{
			ERROR_WRITE(STR("Unexpected record length: ") << returnValue.Length);
			returnValue.Length = 0;
			return returnValue;
		}

		returnValue.Data = new char[returnValue.Length];
		returnValue.Length = returnValue.Length;

		auto bytesRead = 0L;
		Block * currentBlock = block;
		while (true)
		{
			size_t nextBlockId;

			auto currentBlockContentSize = currentBlock->GetHeader(pImpl->kBlockContentLength);
			if (currentBlockContentSize > pImpl->storage->BlockContentSize())
			{
				ERROR_WRITE(STR("Unexpected block content length: ") << currentBlockContentSize);
				return returnValue;
			}

			currentBlock->Read(returnValue.Data, returnValue.Length, bytesRead, 0, currentBlockContentSize);
			bytesRead += currentBlockContentSize;

			nextBlockId = currentBlock->GetHeader(pImpl->kNextBlockId);
			if (nextBlockId == 0)
				return returnValue;

			currentBlock = pImpl->storage->Find(nextBlockId);
			if (currentBlock == NULL)
			{
				ERROR_WRITE(STR("Block not found by id: ") << nextBlockId);
				break;
			}
		}
	}

	return returnValue;
};