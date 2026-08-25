#pragma once

#include "WoW/Source/DB/DBClient/AutoCode/taxinodesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/taxipathrec.h"
#include "WoW/Source/DB/DBClient/dbclient.h"
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/crect.h"

#include <cstdint>
#include <vector>

struct HMODEL__;
struct HTEXTURE__;

struct TAXILINE {
    NTempest::C2Vector src;
    NTempest::C2Vector dst;
    ~TAXILINE();
};

void TaxiMapInitialize();
void TextureUpdateFunc(unsigned width, int height, unsigned depth, unsigned mip,
                       unsigned mipLevel, void* userArg, unsigned* stride,
                       const void** texels);
unsigned UglifyMapTexture();
void TaxiMapShutdown();
HTEXTURE__* TaxiMapGetTexture();
int TaxiMapUpdatePosition(int nodeID, float* worldRect, std::uint64_t reachable,
                          std::uint64_t known);
void FixupRegionRect(float* rect);
unsigned char UpdateTexture(int continent);
void GenerateRouteInfo(int continent, std::uint64_t allNodes);
unsigned TaxiNodeCost(unsigned dstNode, unsigned srcNode);
NTempest::CRect* TaxiMapGetRect(NTempest::CRect* result);
int TaxiNodeGetNodeType(int nodeID);
HMODEL__* TaxiGetRouteModel(float width, float height);
NTempest::C2Vector* CalculateNormalizedCoords(float* result, float* worldPosition);
int TaxiRouteExists(int toNode, int fromNode);

extern NTempest::CRect s_taxiTextureRect;
extern NTempest::CRect s_visibleWorldRect;
extern std::vector<TAXILINE> s_lines;
extern int s_continent;
extern int s_currentTaxiNode;
extern std::uint64_t s_currentReachable;
extern std::uint64_t s_knownNodes;

int _E149_2();
unsigned char* _E148_2();
int _E154_1();
void _E151_1();
int _E153_3();
void _E152_2();
int _E159_2();
void _E156_1();
int _E158_2();
void _E157_1();
