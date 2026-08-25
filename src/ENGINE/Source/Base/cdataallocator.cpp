#include "ENGINE/Source/Base/cdataallocator.h"

#include <algorithm>
#include <cstring>
#include <new>

CDataAllocator::CDataAllocator(unsigned bytesPerData, unsigned dataPerBlock)
    : m_bytesPerData(std::max(bytesPerData, static_cast<unsigned>(sizeof(Data)))),
      m_dataPerBlock(std::max(dataPerBlock, 1u)), m_dataUsed(0), m_blockList(nullptr), m_dataList(nullptr) {}

CDataAllocator::~CDataAllocator() { Clear(); }

void CDataAllocator::Clear() {
    while (m_blockList) {
        Block* next = m_blockList->m_next;
        ::operator delete(m_blockList);
        m_blockList = next;
    }
    m_dataList = nullptr;
    m_dataUsed = 0;
}

void* CDataAllocator::GetData(int zeroData) {
    if (!m_dataList) {
        const std::size_t bytes = sizeof(Block) + static_cast<std::size_t>(m_bytesPerData) * m_dataPerBlock;
        auto* block = static_cast<Block*>(::operator new(bytes));
        block->m_next = m_blockList;
        m_blockList = block;
        auto* cursor = reinterpret_cast<unsigned char*>(block + 1);
        for (unsigned i = 0; i < m_dataPerBlock; ++i) {
            auto* data = reinterpret_cast<Data*>(cursor + static_cast<std::size_t>(i) * m_bytesPerData);
            data->m_next = m_dataList;
            m_dataList = data;
        }
    }
    Data* result = m_dataList;
    m_dataList = result->m_next;
    ++m_dataUsed;
    if (zeroData) std::memset(result, 0, m_bytesPerData);
    return result;
}

void CDataAllocator::PutData(void* data) {
    if (!data) return;
    auto* entry = static_cast<Data*>(data);
    entry->m_next = m_dataList;
    m_dataList = entry;
    if (m_dataUsed) --m_dataUsed;
}
