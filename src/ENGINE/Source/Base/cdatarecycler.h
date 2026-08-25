#pragma once

#include <cstddef>
#include <mutex>

class CDataRecycler {
public:
    struct Node { Node* next; void* data; unsigned bytes; };
    CDataRecycler(unsigned maxNodes, unsigned nodesPerBlock);
    virtual ~CDataRecycler();
    void Clear();
    void* GetData(unsigned* bytes = nullptr);
    void PutData(int* references, void* data, unsigned bytes, const char* file = nullptr, int line = 0);
    virtual void* AllocData(unsigned bytes, const char* file, int line);
    virtual void* ReallocData(void* data, unsigned bytes, const char* file, int line);
    virtual void FreeData(void* data, const char* file, int line);
    static void Link(Node*& list, Node* node);
    static Node* Unlink(Node*& list);
private:
    unsigned m_maxNodes, m_nodesPerBlock, m_nodeCount;
    Node* m_nodeFullList; Node* m_nodeEmptyList;
    std::mutex m_mutex;
};
