#pragma once

#include <cstdint>

struct CClientObjCreate {
    std::uint64_t victim = 0;
};

class CCombat {
public:
    void GetClientInitData(CClientObjCreate* init) const;
    std::uint64_t IsAttacking() const;
    void SetAttacking(std::uint64_t victim);
    void SetClientInitData(const CClientObjCreate* init);

protected:
    std::uint64_t m_victim = 0;
};

class CCombatClient {
public:
    void SetAttackSent(std::uint64_t victim);

    std::uint64_t IsAttacking() const { return m_victim; }

private:
    std::uint64_t m_victim = 0;
    int m_attackSent = 0;
    int m_stopSent = 0;
};

// Recovered symbol declarations for combat.cpp
