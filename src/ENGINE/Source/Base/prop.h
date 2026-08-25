#pragma once

struct HPROPCONTEXT__;
using HPROPCONTEXT=HPROPCONTEXT__*;
void PropInitialize(); void PropDestroy(); HPROPCONTEXT PropCreateContext();
void PropSelectContext(HPROPCONTEXT); HPROPCONTEXT PropGetSelectedContext(); void PropDeleteContext(HPROPCONTEXT);
void* PropGet(unsigned id); void PropSet(unsigned id,void* value);
