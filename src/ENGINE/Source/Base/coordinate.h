#pragma once

// The recovered client stores a normalized rectangle as left/bottom/right/top
// (matching Storm's RGN and the original Scrn layer code).
struct RECTF { float left, bottom, right, top; };
void CoordinateSetDisplaySize(float width, float height);
void NDCToDDC(float* ddcx, float* ddcy, float ndcx, float ndcy);
void NDCToDDC(RECTF* ddc, const RECTF* ndc);
void DDCToNDC(float* ndcx, float* ndcy, float ddcx, float ddcy);
void DDCToNDC(RECTF* ndc, const RECTF* ddc);
double DDCToNDCWidth(float width);
double DDCToNDCHeight(float height);
double NDCToDDCWidth(float width);
double NDCToDDCHeight(float height);
