#pragma once

#include "ENGINE/Source/Tempest/caabox.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

namespace NTempest {

struct C3Segment {
    C3Vector start;
    C3Vector end;
};

} // namespace NTempest

struct CAaBspNode {
    std::uint16_t flags = 0;
    std::uint16_t negChild = 0xFFFF;
    std::uint16_t posChild = 0xFFFF;
    std::uint16_t nFaces = 0;
    unsigned int faceStart = 0;
    float planeDist = 0.0F;
};

class CAaBsp {
public:
    enum Plane : unsigned int {
        Plane_Front = 0,
        Plane_Back = 1,
        Plane_On = 2,
    };

    enum NodeFlags : std::uint16_t {
        Flag_XAxis = 0,
        Flag_YAxis = 1,
        Flag_ZAxis = 2,
        Flag_AxisMask = 3,
        Flag_Leaf = 4,
        Flag_NoChild = 0xFFFF,
    };

    CAaBspNode* rootNode = nullptr;
    CAaBspNode* nodes = nullptr;
    std::uint16_t* nodeFaceIndices = nullptr;
    unsigned int nNodes = 0;
    unsigned int nNodeFaceIndices = 0;
    std::uint16_t* faceVertexIndices = nullptr;
    unsigned int nFaceVertexIndices = 0;
    NTempest::C3Vector* vertices = nullptr;
    unsigned int nVertices = 0;
    unsigned int nodeSize = 0;
    unsigned int nodeNext = 0;
    unsigned int nodeFaceIndicesSize = 0;
    unsigned int nodeFaceIndicesNext = 0;
    std::uint16_t* buildFaceIndices = nullptr;
    unsigned int buildFaceIndicesSize = 0;
    unsigned int buildFaceIndicesNext = 0;
    unsigned int treeDepth = 0;
    unsigned int avgNodeFaces = 0;
    int bFree = 1;
    NTempest::CAaBox aaBox;

    static NTempest::C3Vector s_axisNormalTable[3];

    CAaBsp();
    ~CAaBsp();
    void operator=(const CAaBsp& rhs);

    void Clear();
    void Create(NTempest::C3Vector* vertices, unsigned int nVertices,
                std::uint16_t* faceVertexIndices, unsigned int nFaceVertexIndices);
    void Set(CAaBspNode* nodeList, unsigned int nNodes, std::uint16_t* faceIndices,
             unsigned int nFaceIndices, const NTempest::CAaBox* box);

    unsigned int GetFaceIndices(NTempest::C3Segment& segment,
                                std::uint16_t* indices, unsigned int maxCount);
    unsigned int GetFaceIndices(NTempest::CAaBox& box,
                                std::uint16_t* indices, unsigned int maxCount);

private:
    void Init();
    void Free();
    std::uint16_t* AllocBuildFaceIndices(unsigned int count);
    void FreeBuildFaceIndices(unsigned int count);
    std::uint16_t AllocNode();
    unsigned int AllocNodeFaceIndices(unsigned int count);
    std::uint16_t BuildTree(std::uint16_t* buildFaceIndices, unsigned int count);
    void GenBoundingBox(NTempest::CAaBox* box, std::uint16_t* buildFaceIndices,
                        unsigned int count);
    void ChoosePlane(unsigned int& bestAxis, float& bestDist,
                     std::uint16_t* buildFaceIndices, unsigned int count);
    void PartitionFaceList(unsigned int axis, float dist,
                           std::uint16_t* buildFaceIndices, unsigned int count,
                           std::uint16_t* posIndices, unsigned int& posCount,
                           std::uint16_t* negIndices, unsigned int& negCount);
    void GetFaceIndices(CAaBspNode* node);
    void GetFaceIndices(unsigned int nodeIndex, NTempest::C3Segment& segment);
    void GetFaceIndices(unsigned int nodeIndex, NTempest::CAaBox& box);
};

static_assert(sizeof(CAaBspNode) == 16, "CAaBspNode layout must remain PDB-compatible.");

int _E1_362();
int _E2_362();
void _E4_337();
int _E5_337();
void _E7_337();
int _E8_327();
int _E17_25();
int _E18_10();
void _E20_73();
int _E23_75();
int _E24_31();
void _E21_26();

