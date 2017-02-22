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
};

RecordStorage::~RecordStorage(){
    delete pImpl.release();
};

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

            currentBlock->Read(returnValue.Data, returnValue.Length, (size_t) bytesRead, 0, currentBlockContentSize);
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

vector<Block *> RecordStorage::FindBlocks (size_t recordId) {
    vector<Block *>  blocks;
    auto success = false;

    auto currentBlockId = recordId;
    try {


        do {
            // Grab next block
            auto block = pImpl->storage->Find(currentBlockId);
            if (block == NULL) {
                // Special case: if block #0 never created, then attempt to create it
                if (currentBlockId == 0) {
                    block = pImpl->storage->Create();
                } else {
                    ERROR_WRITE(STR("Block not found by id: ") << currentBlockId);
                    return vector<Block *>();
                }
            }

            blocks.push_back(block);

            // If this is a deleted block then ignore the fuck out of it
            if (1L == block->GetHeader(pImpl->kIsDeleted)) {
                ERROR_WRITE(STR("Block not found: ") << currentBlockId);
                return vector<Block *>();
            }

            // Move next
            currentBlockId = (size_t) block->GetHeader(pImpl->kNextBlockId);
        } while (currentBlockId != 0);

        success = true;
    } catch (std::exception & e)
    { }

    if (!success) {
        auto blocksEnd = blocks.end();
        for (auto it = blocks.begin(); it != blocksEnd; ++it) {
            delete (*it);
        }
    }
    return blocks;
};

void RecordStorage::GetSpaceTrackingBlock (Block *& lastBlock, Block *& secondLastBlock)
{
    lastBlock = NULL;
    secondLastBlock = NULL;

    auto blocks = this->FindBlocks (0);

    if (blocks.size() == 0) {
        ERROR_WRITE(STR("Failed to find blocks of record 0"));
        return;
    }

    lastBlock = blocks[blocks.size() - 1];
    if (blocks.size() > 1)
        secondLastBlock = blocks[blocks.size() - 2];

    if (blocks.size() > 0)
    {
        auto blocksEnd = blocks.end();
        for (auto it = blocks.begin(); it != blocksEnd ; ++it) {
            if ((lastBlock == NULL || (*it) != lastBlock) && (secondLastBlock == NULL || (*it) != secondLastBlock))
                delete *it;
        }
    }
};


void RecordStorage::AppendUInt32ToContent (Block * block, size_t value)
{
    auto contentLength = block->GetHeader (pImpl->kBlockContentLength);

    if ((contentLength % 4) != 0) {
        ERROR_WRITE(STR("Block content length not %4: ") << contentLength);
        return;
    }

    block->Write(intToBits(value), sizeof(size_t), 0, contentLength, 4);
};

void RecordStorage::MarkAsFree (size_t blockId) {
    Block * lastBlock = NULL;
    Block * secondLastBlock = NULL;
    Block * targetBlock = NULL;
    GetSpaceTrackingBlock (lastBlock, secondLastBlock);

    try {
        // Just append a number, if there is some space left
        auto contentLength = lastBlock->GetHeader (pImpl->kBlockContentLength);
        if ((contentLength + 4) <= pImpl->storage->BlockContentSize()) {
            targetBlock = lastBlock;
        }
            // No more fucking space left, allocate new block for writing.
            // Note that we allocate fresh new block, if we reuse it may fuck things up
        else {
            targetBlock = pImpl->storage->Create ();
            targetBlock->SetHeader (pImpl->kPreviousBlockId, lastBlock->Id());

            lastBlock->SetHeader (pImpl->kNextBlockId, targetBlock->Id());

            contentLength = 0;
        }

        // Write!
        AppendUInt32ToContent (targetBlock, blockId);

        // Extend the block length to 4, as we wrote a number
        targetBlock->SetHeader (pImpl->kBlockContentLength, contentLength+4);
    }
    catch(std::exception & e)
    {

    }

    // Always dispose targetBlock
    if (targetBlock != NULL)
        delete targetBlock;
};