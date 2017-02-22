#pragma once

#include <config.h>
#include <core/Utils.h>

#include <iostream>
#include <vector>
#include <core/data/impl/BlockStorage.h>
#include <core/data/impl/Block.h>

using namespace std;
using namespace core;

namespace core {

    namespace data {
        namespace impl {
			struct tRecordInfo {
				size_t Length; 
				char* Data;
				tRecordInfo() : Data(NULL), Length(0){ }
			};

            class RecordStorage
            {
            public:

                RecordStorage(BlockStorage * blockStorage);
                ~RecordStorage();
                void Update (size_t recordId, char* data, size_t dataLen);
				tRecordInfo Find (size_t recordId);
				size_t Create ();
				size_t Create (char* data, size_t dataLen);
                void Delete (size_t recordId);
                void GetSpaceTrackingBlock (Block *& lastBlock, Block *& secondLastBlock);
                void MarkAsFree (size_t blockId);
                size_t ReadUInt32FromTrailingContent (Block * block);
                void AppendUInt32ToContent (Block * block, size_t value);
                bool TryFindFreeBlock (size_t & blockId);
                Block * AllocateBlock ();
                vector<Block *> FindBlocks (size_t recordId);

            private:
                struct impl;
                unique_ptr<impl> pImpl;
            };
        }
    }
}
