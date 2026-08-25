#pragma once

#include <cstddef>

class CDataAllocator {
public:
    struct Block { Block* m_next; };
    struct Data { Data* m_next; };

    CDataAllocator(unsigned bytesPerData, unsigned dataPerBlock);
    ~CDataAllocator();
    void Clear();
    void* GetData(int zeroData = 0);
    void PutData(void* data);

    unsigned m_bytesPerData;
    unsigned m_dataPerBlock;
    unsigned m_dataUsed;
    Block* m_blockList;
    Data* m_dataList;
};
