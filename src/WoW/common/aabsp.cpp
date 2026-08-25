#include "WoW/common/aabsp.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>

namespace {

struct FaceQuery {
    std::uint16_t* indices = nullptr;
    unsigned int maxCount = 0;
    unsigned int count = 0;
    std::array<std::uint8_t, 8192> faceBits{};
};

FaceQuery s_faceQuery;
constexpr std::uint8_t s_faceBitsMask[8] = {1, 2, 4, 8, 16, 32, 64, 128};

bool SameSide(float value) {
    return value > 0.0F;
}

} // namespace

NTempest::C3Vector CAaBsp::s_axisNormalTable[3] = {
    NTempest::C3Vector(1.0F, 0.0F, 0.0F),
    NTempest::C3Vector(0.0F, 1.0F, 0.0F),
    NTempest::C3Vector(0.0F, 0.0F, 1.0F),
};

float INFINITY_362 = 0.0F;
float TWO_PI_325 = 0.0F;
float OO_TWO_PI_326 = 0.0F;

int _E1_362() {
    INFINITY_362 = std::numeric_limits<float>::infinity();
    return 0x7F800000;
}

int _E2_362() { return _E1_362(); }

void _E4_337() { TWO_PI_325 = 3.1415927F + 3.1415927F; }
int _E5_337() { _E4_337(); return 0; }
void _E7_337() { OO_TWO_PI_326 = 1.0F / TWO_PI_325; }
int _E8_327() { _E7_337(); return 0; }

int _E17_25() {
    s_faceQuery.indices = nullptr;
    s_faceQuery.maxCount = 0;
    s_faceQuery.count = 0;
    s_faceQuery.faceBits.fill(0);
    return 0;
}

int _E18_10() { return _E17_25(); }

void _E20_73() {
    CAaBsp::s_axisNormalTable[0] = NTempest::C3Vector(1.0F, 0.0F, 0.0F);
    CAaBsp::s_axisNormalTable[1] = NTempest::C3Vector(0.0F, 1.0F, 0.0F);
    CAaBsp::s_axisNormalTable[2] = NTempest::C3Vector(0.0F, 0.0F, 1.0F);
}

void _E21_26() {}
int _E23_75() { return 0; }
int _E24_31() { _E20_73(); return _E23_75(); }

CAaBsp::CAaBsp() {
    aaBox.b = NTempest::C3Vector(0.0F);
    aaBox.t = NTempest::C3Vector(0.0F);
    Init();
}

CAaBsp::~CAaBsp() { Free(); }

void CAaBsp::operator=(const CAaBsp& rhs) {
    if (this == &rhs) return;
    if (nodes != nullptr || nodeFaceIndices != nullptr) {
        // The original asserts here.  Clear is the recoverable equivalent for
        // the reconstructed library and leaves the same postcondition.
        Free();
        nodes = nullptr;
        nodeFaceIndices = nullptr;
    }

    nNodes = rhs.nNodes;
    nodes = nNodes == 0 ? nullptr : new CAaBspNode[nNodes];
    if (nodes != nullptr) std::memcpy(nodes, rhs.nodes, sizeof(CAaBspNode) * nNodes);
    rootNode = nodes;
    nNodeFaceIndices = rhs.nNodeFaceIndices;
    nodeFaceIndices = nNodeFaceIndices == 0 ? nullptr : new std::uint16_t[nNodeFaceIndices];
    if (nodeFaceIndices != nullptr) {
        std::memcpy(nodeFaceIndices, rhs.nodeFaceIndices,
                    sizeof(std::uint16_t) * nNodeFaceIndices);
    }
    faceVertexIndices = rhs.faceVertexIndices;
    nFaceVertexIndices = rhs.nFaceVertexIndices;
    vertices = rhs.vertices;
    nVertices = rhs.nVertices;
    nodeSize = rhs.nodeSize;
    nodeNext = rhs.nodeNext;
    nodeFaceIndicesSize = rhs.nodeFaceIndicesSize;
    nodeFaceIndicesNext = rhs.nodeFaceIndicesNext;
    buildFaceIndices = nullptr;
    buildFaceIndicesSize = 0;
    buildFaceIndicesNext = 0;
    treeDepth = rhs.treeDepth;
    avgNodeFaces = rhs.avgNodeFaces;
    bFree = 1;
    aaBox = rhs.aaBox;
}

void CAaBsp::Clear() {
    Free();
    Init();
}

void CAaBsp::Create(NTempest::C3Vector* newVertices, unsigned int vertexCount,
                    std::uint16_t* newFaceVertexIndices, unsigned int faceIndexCount) {
    if (newVertices == nullptr || newFaceVertexIndices == nullptr) return;
    if (vertexCount >= 0x40000 || faceIndexCount >= 0x40000) return;

    vertices = newVertices;
    nVertices = vertexCount;
    faceVertexIndices = newFaceVertexIndices;
    nFaceVertexIndices = faceIndexCount;
    bFree = 1;
    NTempest::CAaBox bounds;
    bounds.Bounding(vertices, nVertices);
    aaBox = bounds;

    if (nodes == nullptr) nodes = new CAaBspNode[0x10000];
    if (nodeFaceIndices == nullptr) nodeFaceIndices = new std::uint16_t[0x40000];
    if (buildFaceIndices == nullptr) buildFaceIndices = new std::uint16_t[0x200000];
    nodeSize = 0x10000;
    nodeNext = 0;
    nNodes = 0;
    nodeFaceIndicesSize = 0x40000;
    nodeFaceIndicesNext = 0;
    nNodeFaceIndices = 0;
    buildFaceIndicesSize = 0x200000;
    buildFaceIndicesNext = 0;
    treeDepth = 0;
    avgNodeFaces = 0;

    const unsigned int faceCount = faceIndexCount / 3;
    std::uint16_t* buildIndices = AllocBuildFaceIndices(faceCount);
    for (unsigned int i = 0; i < faceCount; ++i) buildIndices[i] = static_cast<std::uint16_t>(i);
    if (faceCount != 0) BuildTree(buildIndices, faceCount);
    FreeBuildFaceIndices(faceCount);
    rootNode = nodes;
    avgNodeFaces = nNodes == 0 ? 0 : avgNodeFaces / nNodes;
}

void CAaBsp::Set(CAaBspNode* nodeList, unsigned int nodeCount,
                 std::uint16_t* faceIndices, unsigned int faceCount,
                 const NTempest::CAaBox* box) {
    if (nodeList == nullptr || faceIndices == nullptr || box == nullptr) return;
    nodes = nodeList;
    rootNode = nodeList;
    nodeFaceIndices = faceIndices;
    nNodeFaceIndices = faceCount;
    bFree = 0;
    nNodes = nodeCount;
    aaBox = *box;
}

unsigned int CAaBsp::GetFaceIndices(NTempest::C3Segment& segment,
                                    std::uint16_t* indices, unsigned int maxCount) {
    s_faceQuery.indices = indices;
    s_faceQuery.maxCount = maxCount;
    s_faceQuery.count = 0;
    s_faceQuery.faceBits.fill(0);
    if (rootNode != nullptr && nodes != nullptr) GetFaceIndices(0, segment);
    for (unsigned int i = 0; i < s_faceQuery.count; ++i) {
        const std::uint16_t face = s_faceQuery.indices[i];
        s_faceQuery.faceBits[face >> 3] &= static_cast<std::uint8_t>(~s_faceBitsMask[face & 7]);
    }
    return s_faceQuery.count;
}

unsigned int CAaBsp::GetFaceIndices(NTempest::CAaBox& box,
                                    std::uint16_t* indices, unsigned int maxCount) {
    s_faceQuery.indices = indices;
    s_faceQuery.maxCount = maxCount;
    s_faceQuery.count = 0;
    s_faceQuery.faceBits.fill(0);
    if (rootNode != nullptr && nodes != nullptr) GetFaceIndices(0, box);
    for (unsigned int i = 0; i < s_faceQuery.count; ++i) {
        const std::uint16_t face = s_faceQuery.indices[i];
        s_faceQuery.faceBits[face >> 3] &= static_cast<std::uint8_t>(~s_faceBitsMask[face & 7]);
    }
    return s_faceQuery.count;
}

void CAaBsp::Init() {
    rootNode = nullptr;
    nodes = nullptr;
    nodeFaceIndices = nullptr;
    nNodes = 0;
    nNodeFaceIndices = 0;
    faceVertexIndices = nullptr;
    nFaceVertexIndices = 0;
    vertices = nullptr;
    nVertices = 0;
    nodeSize = 0;
    nodeNext = 0;
    nodeFaceIndicesSize = 0;
    nodeFaceIndicesNext = 0;
    buildFaceIndices = nullptr;
    buildFaceIndicesSize = 0;
    buildFaceIndicesNext = 0;
    treeDepth = 0;
    avgNodeFaces = 0;
    bFree = 1;
}

void CAaBsp::Free() {
    if (bFree != 0) {
        delete[] nodes;
        delete[] nodeFaceIndices;
    }
    delete[] buildFaceIndices;
}

std::uint16_t* CAaBsp::AllocBuildFaceIndices(unsigned int count) {
    if (count + buildFaceIndicesNext >= buildFaceIndicesSize || buildFaceIndices == nullptr) return nullptr;
    std::uint16_t* result = &buildFaceIndices[buildFaceIndicesNext];
    buildFaceIndicesNext += count;
    return result;
}

void CAaBsp::FreeBuildFaceIndices(unsigned int count) {
    buildFaceIndicesNext -= std::min(count, buildFaceIndicesNext);
}

std::uint16_t CAaBsp::AllocNode() {
    if (nNodes >= 0x10000 || nodes == nullptr) return Flag_NoChild;
    const std::uint16_t result = static_cast<std::uint16_t>(nodeNext++);
    ++nNodes;
    return result;
}

unsigned int CAaBsp::AllocNodeFaceIndices(unsigned int count) {
    if (count + nodeFaceIndicesNext >= nodeFaceIndicesSize || nodeFaceIndices == nullptr) return 0;
    const unsigned int result = nodeFaceIndicesNext;
    nodeFaceIndicesNext += count;
    nNodeFaceIndices += count;
    return result;
}

std::uint16_t CAaBsp::BuildTree(std::uint16_t* faceIndices, unsigned int count) {
    if (treeDepth >= 8 || count <= 0x10) {
        const std::uint16_t leafIndex = AllocNode();
        if (leafIndex == Flag_NoChild) return leafIndex;
        CAaBspNode& node = nodes[leafIndex];
        node.flags = Flag_Leaf;
        node.posChild = Flag_NoChild;
        node.negChild = Flag_NoChild;
        node.faceStart = AllocNodeFaceIndices(count);
        node.nFaces = static_cast<std::uint16_t>(count);
        node.planeDist = 0.0F;
        for (unsigned int i = 0; i < count; ++i) nodeFaceIndices[node.faceStart + i] = faceIndices[i];
        avgNodeFaces += count;
        return leafIndex;
    }

    unsigned int axis = 0;
    float dist = 0.0F;
    ChoosePlane(axis, dist, faceIndices, count);
    const std::uint16_t nodeIndex = AllocNode();
    if (nodeIndex == Flag_NoChild) return nodeIndex;
    CAaBspNode& node = nodes[nodeIndex];
    node.flags = static_cast<std::uint16_t>(axis);
    node.planeDist = dist;
    node.faceStart = 0;
    node.nFaces = 0;

    std::uint16_t* posIndices = AllocBuildFaceIndices(count);
    std::uint16_t* negIndices = AllocBuildFaceIndices(count);
    if (posIndices == nullptr || negIndices == nullptr) return nodeIndex;
    unsigned int posCount = 0;
    unsigned int negCount = 0;
    PartitionFaceList(axis, dist, faceIndices, count, posIndices, posCount,
                      negIndices, negCount);

    ++treeDepth;
    node.posChild = posCount == 0 ? Flag_NoChild : BuildTree(posIndices, posCount);
    node.negChild = negCount == 0 ? Flag_NoChild : BuildTree(negIndices, negCount);
    --treeDepth;
    FreeBuildFaceIndices(count);
    FreeBuildFaceIndices(count);
    return nodeIndex;
}

void CAaBsp::GenBoundingBox(NTempest::CAaBox* box,
                            std::uint16_t* faceIndices, unsigned int count) {
    const float high = std::numeric_limits<float>::max();
    box->b = NTempest::C3Vector(high);
    box->t = NTempest::C3Vector(-high);
    for (unsigned int face = 0; face < count; ++face) {
        const unsigned int first = 3U * faceIndices[face];
        for (unsigned int vertex = 0; vertex < 3; ++vertex) {
            const NTempest::C3Vector& value = vertices[faceVertexIndices[first + vertex]];
            box->b.x = std::min(box->b.x, value.x);
            box->b.y = std::min(box->b.y, value.y);
            box->b.z = std::min(box->b.z, value.z);
            box->t.x = std::max(box->t.x, value.x);
            box->t.y = std::max(box->t.y, value.y);
            box->t.z = std::max(box->t.z, value.z);
        }
    }
}

void CAaBsp::ChoosePlane(unsigned int& bestAxis, float& bestDist,
                         std::uint16_t* faceIndices, unsigned int count) {
    NTempest::CAaBox box;
    GenBoundingBox(&box, faceIndices, count);
    float bestScore = 999999.88F;
    bestAxis = 0;
    bestDist = 0.0F;

    for (unsigned int axis = 0; axis < 3; ++axis) {
        const int begin = static_cast<int>(box.b[axis] - 0.5F);
        const int end = static_cast<int>(box.t[axis] - 0.5F);
        const float range = (box.t[axis] - box.b[axis]) * 0.0625F;
        const int step = std::max(1, static_cast<int>(range - 0.5F));
        for (int distance = begin; distance <= end; distance += step) {
            unsigned int front = 0;
            unsigned int back = 0;
            unsigned int split = 0;
            unsigned int on = 0;
            for (unsigned int face = 0; face < count; ++face) {
                const unsigned int first = 3U * faceIndices[face];
                unsigned int nonPositive = 0;
                unsigned int nonNegative = 0;
                for (unsigned int vertex = 0; vertex < 3; ++vertex) {
                    const float value = vertices[faceVertexIndices[first + vertex]][axis] - static_cast<float>(distance);
                    if (value <= 0.0F) ++nonPositive;
                    if (value >= 0.0F) ++nonNegative;
                }
                if (nonPositive == 3 && nonNegative == 3) ++on;
                else if (nonNegative == 3) ++front;
                else if (nonPositive == 3) ++back;
                else ++split;
            }
            const float score = std::fabs(static_cast<float>(front) - static_cast<float>(back))
                              + static_cast<float>(2 * split) + static_cast<float>(on);
            if (score != 0.0F && score < bestScore) {
                bestScore = score;
                bestAxis = axis;
                bestDist = static_cast<float>(distance);
            }
        }
    }
}

void CAaBsp::PartitionFaceList(unsigned int axis, float dist,
                               std::uint16_t* faceIndices, unsigned int count,
                               std::uint16_t* posIndices, unsigned int& posCount,
                               std::uint16_t* negIndices, unsigned int& negCount) {
    posCount = 0;
    negCount = 0;
    for (unsigned int face = 0; face < count; ++face) {
        const std::uint16_t faceIndex = faceIndices[face];
        const unsigned int first = 3U * faceIndex;
        unsigned int nonPositive = 0;
        unsigned int nonNegative = 0;
        for (unsigned int vertex = 0; vertex < 3; ++vertex) {
            const float value = vertices[faceVertexIndices[first + vertex]][axis] - dist;
            if (value <= 0.0F) ++nonPositive;
            if (value >= 0.0F) ++nonNegative;
        }
        if (nonPositive == 0) {
            posIndices[posCount++] = faceIndex;
        } else if (nonNegative == 0) {
            negIndices[negCount++] = faceIndex;
        } else {
            posIndices[posCount++] = faceIndex;
            negIndices[negCount++] = faceIndex;
        }
    }
}

void CAaBsp::GetFaceIndices(CAaBspNode* node) {
    if (node == nullptr || s_faceQuery.indices == nullptr) return;
    for (unsigned int i = 0; i < node->nFaces; ++i) {
        const std::uint16_t face = nodeFaceIndices[node->faceStart + i];
        const std::uint8_t mask = s_faceBitsMask[face & 7];
        if ((s_faceQuery.faceBits[face >> 3] & mask) != 0) continue;
        if (s_faceQuery.count >= s_faceQuery.maxCount) return;
        s_faceQuery.indices[s_faceQuery.count++] = face;
        s_faceQuery.faceBits[face >> 3] |= mask;
    }
}

void CAaBsp::GetFaceIndices(unsigned int nodeIndex, NTempest::C3Segment& segment) {
    if (nodeIndex >= nNodes) return;
    const CAaBspNode& node = nodes[nodeIndex];
    if ((node.flags & Flag_Leaf) != 0) {
        GetFaceIndices(const_cast<CAaBspNode*>(&node));
        return;
    }
    const unsigned int axis = node.flags & Flag_AxisMask;
    const float d0 = segment.start[axis] - node.planeDist;
    const float d1 = segment.end[axis] - node.planeDist;
    if (d0 == 0.0F && d1 == 0.0F) {
        if (node.posChild != Flag_NoChild) GetFaceIndices(node.posChild, segment);
        if (node.negChild != Flag_NoChild) GetFaceIndices(node.negChild, segment);
        return;
    }
    if ((d0 < 0.0F) == (d1 < 0.0F)) {
        const std::uint16_t child = d0 > 0.0F ? node.posChild : node.negChild;
        if (child != Flag_NoChild) GetFaceIndices(child, segment);
        return;
    }

    const float fraction = d0 / (d0 - d1);
    const NTempest::C3Vector midpoint = segment.start + (segment.end - segment.start) * fraction;
    NTempest::C3Segment first = segment;
    NTempest::C3Segment second = segment;
    first.end = midpoint;
    second.start = midpoint;
    if (d0 <= 0.0F) {
        if (node.negChild != Flag_NoChild) GetFaceIndices(node.negChild, first);
        if (node.posChild != Flag_NoChild) GetFaceIndices(node.posChild, second);
    } else {
        if (node.posChild != Flag_NoChild) GetFaceIndices(node.posChild, first);
        if (node.negChild != Flag_NoChild) GetFaceIndices(node.negChild, second);
    }
}

void CAaBsp::GetFaceIndices(unsigned int nodeIndex, NTempest::CAaBox& box) {
    if (nodeIndex >= nNodes) return;
    const CAaBspNode& node = nodes[nodeIndex];
    if ((node.flags & Flag_Leaf) != 0) {
        GetFaceIndices(const_cast<CAaBspNode*>(&node));
        return;
    }
    const unsigned int axis = node.flags & Flag_AxisMask;
    if (box.b[axis] > node.planeDist) {
        if (node.posChild != Flag_NoChild) GetFaceIndices(node.posChild, box);
        return;
    }
    if (box.t[axis] < node.planeDist) {
        if (node.negChild != Flag_NoChild) GetFaceIndices(node.negChild, box);
        return;
    }
    if (node.posChild != Flag_NoChild) {
        NTempest::CAaBox positive = box;
        positive.b[axis] = node.planeDist;
        GetFaceIndices(node.posChild, positive);
    }
    if (node.negChild != Flag_NoChild) {
        NTempest::CAaBox negative = box;
        negative.t[axis] = node.planeDist;
        GetFaceIndices(node.negChild, negative);
    }
}

