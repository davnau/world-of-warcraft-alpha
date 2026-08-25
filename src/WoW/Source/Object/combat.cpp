#include "WoW/Source/Object/combat.h"

void CCombat::SetAttacking(std::uint64_t victim) { m_victim = victim; }
void CCombat::GetClientInitData(CClientObjCreate* init) const {
    if (init) init->victim = m_victim;
}
void CCombat::SetClientInitData(const CClientObjCreate* init) {
    if (init) m_victim = init->victim;
}
std::uint64_t CCombat::IsAttacking() const { return m_victim; }
void CCombatClient::SetAttackSent(std::uint64_t victim) {
    m_victim = victim;
    m_attackSent = 1;
}
