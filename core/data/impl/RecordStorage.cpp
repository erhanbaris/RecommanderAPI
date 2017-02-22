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

		auto totalRecordSize = block->GetHeader(pImpl->kRecordLength);

		if (totalRecordSize > pImpl->MaxRecordSize)
		{
			ERROR_WRITE(STR("Unexpected record length: ") << totalRecordSize);
			return returnValue;
		}

		auto * data = new char[totalRecordSize];
		auto bytesRead = 0L;
		Block * currentBlock = block;
		while (true)
		{
			size_t nextBlockId;
		}
	}

	return returnValue;
};