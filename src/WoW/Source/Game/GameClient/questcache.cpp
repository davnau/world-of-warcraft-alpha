#include "WoW/Source/Game/GameClient/questcache.h"

void QuestCache::Pack(CDataStore* message) {
    if (message == nullptr) return;
    message->Put(m_questId)->Put(m_questType)->Put(m_questLevel)->Put(m_questSortID)
        ->Put(m_questInfoID)->Put(m_rewardNextQuest)->Put(m_rewardMoney)->Put(m_startItem);

    unsigned count = 0;
    for (unsigned i = 0; i < 4; ++i) {
        if (m_rewardItems[i] != 0) {
            message->Put(m_rewardItems[i])->Put(m_rewardAmount[i]);
            ++count;
        }
    }
    while (count++ < 4) message->Put(0)->Put(0);

    count = 0;
    for (unsigned i = 0; i < 6; ++i) {
        if (m_rewardChoiceItems[i] != 0) {
            message->Put(m_rewardChoiceItems[i])->Put(m_rewardChoiceAmount[i]);
            ++count;
        }
    }
    while (count++ < 6) message->Put(0)->Put(0);

    message->Put(m_POIContinent)->Put(m_POIx)->Put(m_POIy)->Put(m_POIPriority)
        ->PutString(m_logTitle)->PutString(m_logDescription)
        ->PutString(m_questDescription)->PutString(m_areaDescription);

    for (unsigned i = 0; i < 4; ++i) {
        message->Put(m_monsterToKill[i])->Put(m_monsterToKillQuantity[i])
            ->Put(m_monsterToKillCurrency[i])->Put(m_monsterToKillItem[i])
            ->PutString(m_getDescription[i]);
    }
}

void QuestCache::Unpack(CDataStore* message) {
    if (message == nullptr) return;
    message->Get(&m_questId)->Get(&m_questType)->Get(&m_questLevel)->Get(&m_questSortID)
        ->Get(&m_questInfoID)->Get(&m_rewardNextQuest)->Get(&m_rewardMoney)->Get(&m_startItem);
    for (int i = 0; i < 4; ++i) message->Get(&m_rewardItems[i])->Get(&m_rewardAmount[i]);
    for (int i = 0; i < 6; ++i) message->Get(&m_rewardChoiceItems[i])->Get(&m_rewardChoiceAmount[i]);
    message->Get(&m_POIContinent)->Get(&m_POIx)->Get(&m_POIy)->Get(&m_POIPriority)
        ->GetString(m_logTitle, sizeof(m_logTitle))->GetString(m_logDescription, sizeof(m_logDescription))
        ->GetString(m_questDescription, sizeof(m_questDescription))->GetString(m_areaDescription, sizeof(m_areaDescription));
    for (unsigned i = 0; i < 4; ++i) {
        message->Get(&m_monsterToKill[i])->Get(&m_monsterToKillQuantity[i])
            ->Get(&m_monsterToKillCurrency[i])->Get(&m_monsterToKillItem[i])
            ->GetString(m_getDescription[i], sizeof(m_getDescription[i]));
    }
}
