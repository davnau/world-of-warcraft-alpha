#include "WoW/Source/Magic/spellnet.h"

#include <limits>

float INFINITY_471 = 0.0F;
float TWO_PI_428 = 0.0F;
float OO_TWO_PI_429 = 0.0F;

void SpellPutCastTargets(SpellCast* spell, CDataStore* message) {
    if (spell == nullptr || message == nullptr)
        return;
    message->Put(spell->targets);
    if ((spell->targets & 0x0802u) != 0)
        message->Put(spell->unitTarget);
    if ((spell->targets & 0x1010u) != 0)
        message->Put(spell->itemTarget);
    if ((spell->targets & 0x0020u) != 0)
        message->PutArray(&spell->sourceLocation.x, 3);
    if ((spell->targets & 0x0040u) != 0)
        message->PutArray(&spell->destLocation.x, 3);
    if ((spell->targets & 0x2000u) != 0)
        message->PutArray(spell->targetString, 0x80u);
}

void SpellGetCastTargets(SpellCast* spell, CDataStore* message) {
    if (spell == nullptr || message == nullptr)
        return;
    message->Get(&spell->targets);
    if ((spell->targets & 0x0802u) != 0)
        message->Get(&spell->unitTarget);
    if ((spell->targets & 0x1010u) != 0)
        message->Get(&spell->itemTarget);
    if ((spell->targets & 0x0020u) != 0)
        message->GetArray(&spell->sourceLocation.x, 3);
    if ((spell->targets & 0x0040u) != 0)
        message->GetArray(&spell->destLocation.x, 3);
    if ((spell->targets & 0x2000u) != 0)
        message->GetArray(spell->targetString, 0x80u);
}

int _E2_471() { return _E1_471(); }
int _E1_471() { INFINITY_471 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_441() { _E4_441(); return 0; }
void _E4_441() { TWO_PI_428 = 3.1415927F + 3.1415927F; }
int _E8_430() { _E7_441(); return 0; }
void _E7_441() { OO_TWO_PI_429 = 1.0F / TWO_PI_428; }
