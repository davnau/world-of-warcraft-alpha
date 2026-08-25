#include "WoW/Source/Object/petition.h"

#include <algorithm>

void CGPetition::Pack(CDataStore* msg) const {
    if (!msg) return;
    msg->Put(m_petitionID);
    msg->Put(m_petitioner);
    msg->PutString(m_title);
    msg->PutString(m_bodyText);
    msg->Put(m_flags);
    msg->Put(m_minSignatures);
    msg->Put(m_maxSignatures);
    msg->Put(m_deadLine);
    msg->Put(m_issueDate);
    msg->Put(m_allowedGuildID);
    msg->Put(m_allowedClasses);
    msg->Put(m_allowedRaces);
    msg->Put(m_allowedGender);
    msg->Put(m_allowedMinLevel);
    msg->Put(m_allowedMaxLevel);
    msg->Put(m_numChoices);
    const int choices = std::clamp(m_numChoices, 0, 10);
    for (int i = 0; i < choices; ++i) msg->PutString(m_choicetext[i]);
    msg->Put(m_muid);
}

void CGPetition::Unpack(CDataStore* msg) {
    if (!msg) return;
    msg->Get(&m_petitionID);
    msg->Get(&m_petitioner);
    msg->GetString(m_title, sizeof(m_title));
    msg->GetString(m_bodyText, sizeof(m_bodyText));
    msg->Get(&m_flags);
    msg->Get(&m_minSignatures);
    msg->Get(&m_maxSignatures);
    msg->Get(&m_deadLine);
    msg->Get(&m_issueDate);
    msg->Get(&m_allowedGuildID);
    msg->Get(&m_allowedClasses);
    msg->Get(&m_allowedRaces);
    msg->Get(&m_allowedGender);
    msg->Get(&m_allowedMinLevel);
    msg->Get(&m_allowedMaxLevel);
    msg->Get(&m_numChoices);
    const int choices = std::clamp(m_numChoices, 0, 10);
    for (int i = 0; i < choices; ++i)
        msg->GetString(m_choicetext[i], sizeof(m_choicetext[i]));
    msg->Get(&m_muid);
}
