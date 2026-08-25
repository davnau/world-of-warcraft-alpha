#include "ENGINE/Source/Base/coordinate.h"

namespace {
constexpr float kDdcHeight = 0.60000002f;
float s_ddcWidth = 0.80000001f;
float s_ddcHeight = kDdcHeight;
}

void CoordinateSetDisplaySize(float width,float height) {
    if(width<=0.0f||height<=0.0f)return;
    s_ddcHeight=kDdcHeight;
    s_ddcWidth=s_ddcHeight*(width/height);
}
void NDCToDDC(float* x,float* y,float nx,float ny){if(x)*x=nx*s_ddcWidth;if(y)*y=ny*s_ddcHeight;}
void DDCToNDC(float* x,float* y,float dx,float dy){if(x)*x=dx/s_ddcWidth;if(y)*y=dy/s_ddcHeight;}
void NDCToDDC(RECTF* d,const RECTF* n){if(!d||!n)return;d->left=n->left*s_ddcWidth;d->right=n->right*s_ddcWidth;d->top=n->bottom*s_ddcHeight;d->bottom=n->top*s_ddcHeight;}
void DDCToNDC(RECTF* n,const RECTF* d){if(!d||!n)return;n->left=d->left/s_ddcWidth;n->right=d->right/s_ddcWidth;n->top=d->bottom/s_ddcHeight;n->bottom=d->top/s_ddcHeight;}
double DDCToNDCWidth(float v){return v/s_ddcWidth;}
double DDCToNDCHeight(float v){return v/s_ddcHeight;}
double NDCToDDCWidth(float v){return v*s_ddcWidth;}
double NDCToDDCHeight(float v){return v*s_ddcHeight;}
