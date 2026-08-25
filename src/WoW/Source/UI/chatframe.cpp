#include "WoW/Source/UI/chatframe.h"

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/Glue/charselectinfo.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

extern "C" {
#include "lauxlib.h"
}

#include <array>
#include <cstdio>
#include <cstring>

namespace {
enum SlashCommandId : int {
    SLASH_CMD_SAY = 0x00,
    SLASH_CMD_PARTY = 0x01,
    SLASH_CMD_GUILD = 0x02,
    SLASH_CMD_OFFICER = 0x03,
    SLASH_CMD_YELL = 0x04,
    SLASH_CMD_WHISPER = 0x05,
    SLASH_CMD_EMOTE = 0x07,
    SLASH_CMD_SEND_CHANNEL = 0x0D,
    SLASH_CMD_SEND_AFK = 0x13,
    SLASH_CMD_SEND_DND = 0x14,
};

unsigned DefaultLanguage() {
    // Languages.dbc IDs used by this client.  This is the recovered
    // CGPlayer_C::GetDefaultLanguage result expressed through the character
    // record that is currently available in the reconstructed object path.
    const unsigned race = CGlueMgr::m_characterInfo
        ? CGlueMgr::m_characterInfo->raceID : 1U;
    switch (race) {
    case 2: return 1;   // Orcish
    case 4: return 2;   // Darnassian
    case 6: return 3;   // Taurahe
    case 8: return 14;  // Troll
    default: return 7;  // Common
    }
}

unsigned ChatEventId(int type) {
    if (type >= 0 && type <= 24) return 0xD9U + static_cast<unsigned>(type);
    if (type >= 25 && type <= 29) return 0x152U + static_cast<unsigned>(type - 25);
    return ~0U;
}

const char* LanguageName(unsigned language) {
    switch (language) {
    case 0: return "Universal";
    case 1: return "Orcish";
    case 2: return "Darnassian";
    case 3: return "Taurahe";
    case 6: return "Dwarvish";
    case 7: return "Common";
    case 8: return "Demonic";
    case 9: return "Titanic";
    case 10: return "Thalassian";
    case 11: return "Draconic";
    case 12: return "Kalimag";
    case 13: return "Gnomish";
    case 14: return "Troll";
    default: return "";
    }
}
}

void CGChat::AddChannel() { }
void CGChat::AddChatMessage(const char* text, int type, const char* player,
                            unsigned language, const char* channel,
                            const char* player2, const char* specialFlag) {
    const unsigned eventId = ChatEventId(type);
    if (eventId == ~0U) return;
    FrameScript_SignalEvent(eventId, "%s%s%s%s%s%s",
        text ? text : "", player ? player : "", LanguageName(language),
        channel ? channel : "", player2 ? player2 : "",
        specialFlag ? specialFlag : "");
}
void CGChat::AddTextEmoteMessage() { }
void CGChat::ChannelList() { }
void CGChat::ChannelNotify() { }
int CGChat::ChatHandler(CDataStore* message) {
    if (!message) return 0;

    std::uint8_t type = 0;
    std::uint32_t language = 0;
    std::uint64_t sender = 0;
    std::uint8_t status = 0;
    char text[512]{};
    char channel[128]{};
    char monsterName[48]{};

    message->Get(&type);
    message->Get(&language);
    if (type == 10 || type == 11 || type == 12) {
        message->GetString(monsterName, sizeof(monsterName));
        message->Get(&sender);
        message->GetString(text, sizeof(text));
        message->Get(&status);
    } else {
        if (type == SLASH_CMD_SEND_CHANNEL)
            message->GetString(channel, sizeof(channel));
        message->Get(&sender);
        message->GetString(text, sizeof(text));
        message->Get(&status);
    }

    if (message->Failed() || !message->IsRead()) {
        std::fprintf(stderr, "CHAT_RECV parse_failed size=%u read=%u type=%u language=%u\n",
                     message->m_size, message->m_read, static_cast<unsigned>(type), language);
        message->Reset();
        return 1;
    }

    const char* player = monsterName[0] ? monsterName : nullptr;
    if (!player && sender != 0 && CGlueMgr::m_characterInfo &&
        sender == CGlueMgr::m_characterInfo->guid)
        player = CGlueMgr::m_characterInfo->name;
    if (!player && sender != 0) player = "Unknown";

    const char* specialFlag = "";
    if (status == 1) specialFlag = "AFK";
    else if (status == 2) specialFlag = "DND";
    else if (status == 3) specialFlag = "GM";

    std::fprintf(stderr,
                 "CHAT_RECV type=%u language=%u sender=%llu channel=%s text=%s status=%u\n",
                 static_cast<unsigned>(type), language,
                 static_cast<unsigned long long>(sender), channel, text,
                 static_cast<unsigned>(status));
    AddChatMessage(text, type, player, language, channel, nullptr, specialFlag);
    return 1;
}
void CGChat::CheckFlagChanged() { }
void CGChat::DisplayPendingUserList() { }
void CGChat::EnterWorld() { }
void CGChat::GetChannel() { }
void CGChat::GetChannelID() { }
void CGChat::GetChannelName() { }
void CGChat::GetChannelString() { }
void CGChat::GetPendingChatMessages() { }
void CGChat::HandleFlagsChanged() { }
void CGChat::HandleTextEmote() { }
void CGChat::InitializeGame() { }
void CGChat::LeaveWorld() { }
void CGChat::NameQueryCallback() { }
void CGChat::QueueChatText() { }
void CGChat::QueueTextEmote() { }
void CGChat::RemoveChannel() { }
void CGChat::ShutdownGame() { }
void CGChat::TextEmoteNameQueryCallback() { }
void CGChat::TranslateMessage() { }
void CGChat::UpdateLanguages() { }
void ChannelCommand() { }
void ChannelPlayerCommand() { }
void ChatRegisterScriptFunctions() {
    FrameScript_RegisterFunction("SendChatMessage", Script_SendChatMessage);
}
void ChatUnregisterScriptFunctions() {
    FrameScript_UnregisterFunction("SendChatMessage");
}
void CopyWordCase() { }
void Script_ChannelBan() { }
void Script_ChannelInvite() { }
void Script_ChannelKick() { }
void Script_ChannelModerate() { }
void Script_ChannelModerator() { }
void Script_ChannelMute() { }
void Script_ChannelToggleAnnouncements() { }
void Script_ChannelUnban() { }
void Script_ChannelUnmoderator() { }
void Script_ChannelUnmute() { }
void Script_ChatFrameLog() { }
void Script_DisplayChannelOwner() { }
void Script_DoEmote() { }
void Script_GetChannelName() { }
void Script_GetDefaultLanguage() { }
void Script_GetLanguageByIndex() { }
void Script_GetNumLanguages() { }
void Script_JoinChannelByName() { }
void Script_LeaveChannelByName() { }
void Script_ListChannelByName() { }
void Script_ListChannels() { }
int Script_SendChatMessage(lua_State* state) {
    // Script_SendChatMessage at 0x4ED080: active-player guard, text/type/
    // language/target decoding, then CMSG_MESSAGECHAT (0x95).
    if (!state) {
        std::fprintf(stderr, "CHAT_SEND skipped reason=no_lua_state\n");
        return 0;
    }
    if (ClntObjMgrGetActivePlayer() == 0U) {
        std::fprintf(stderr, "CHAT_SEND skipped reason=no_active_player\n");
        return 0;
    }
    if (!lua_isstring(state, 1))
        return luaL_error(state,
            "Usage: SendChatMessage(text [,type] [,language] [,target])");

    const char* text = lua_tostring(state, 1);
    if (!text || !*text) return 0;

    int type = SLASH_CMD_SAY;
    if (lua_isstring(state, 2)) {
        const char* name = lua_tostring(state, 2);
        if (!StringToChatType(name, &type))
            return luaL_error(state, "Unknown chat type: %s", name ? name : "");
    }

    unsigned language = DefaultLanguage();
    if (lua_isstring(state, 3)) {
        const char* name = lua_tostring(state, 3);
        if (!StringToLanguage(name, &language))
            return luaL_error(state, "Unknown language: %s", name ? name : "");
    }

    const char* target = lua_isstring(state, 4) ? lua_tostring(state, 4) : nullptr;
    if ((type == SLASH_CMD_WHISPER || type == SLASH_CMD_SEND_CHANNEL) &&
        (!target || !*target))
        return 0;

    CDataStore message;
    message.Put<std::uint32_t>(0x95U);
    message.Put<std::int32_t>(type);
    message.Put<std::uint32_t>(language);
    if (type == SLASH_CMD_WHISPER || type == SLASH_CMD_SEND_CHANNEL)
        message.PutString(target);
    message.PutString(text);
    std::fprintf(stderr, "CHAT_SEND type=%d language=%u target=%s text=%s bytes=%u\n",
                 type, language, target ? target : "", text, message.m_size);
    ClientServices_Send(&message);
    return 0;
}
void Script_SetChannelOwner() { }
void Script_SetChannelPassword() { }
void SendChatEvent() { }
int StringToChatType(const char* name, int* type) {
    struct Entry { int id; const char* name; };
    static constexpr Entry entries[] = {
        {SLASH_CMD_SAY, "SAY"}, {SLASH_CMD_PARTY, "PARTY"},
        {SLASH_CMD_GUILD, "GUILD"}, {SLASH_CMD_OFFICER, "OFFICER"},
        {SLASH_CMD_YELL, "YELL"}, {SLASH_CMD_WHISPER, "WHISPER"},
        {SLASH_CMD_EMOTE, "EMOTE"}, {SLASH_CMD_SEND_CHANNEL, "CHANNEL"},
        {SLASH_CMD_SEND_AFK, "AFK"}, {SLASH_CMD_SEND_DND, "DND"},
    };
    if (!name || !type) return 0;
    for (const auto& entry : entries) {
        if (_stricmp(name, entry.name) == 0) {
            *type = entry.id;
            return 1;
        }
    }
    return 0;
}

int StringToLanguage(const char* name, unsigned* language) {
    struct Entry { unsigned id; const char* name; };
    static constexpr Entry entries[] = {
        {0, "Universal"}, {1, "Orcish"}, {2, "Darnassian"},
        {3, "Taurahe"}, {6, "Dwarvish"}, {7, "Common"},
        {8, "Demonic"}, {9, "Titanic"}, {10, "Thalassian"},
        {11, "Draconic"}, {12, "Kalimag"}, {13, "Gnomish"},
        {14, "Troll"},
    };
    if (!name || !language) return 0;
    for (const auto& entry : entries) {
        if (_stricmp(name, entry.name) == 0) {
            *language = entry.id;
            return 1;
        }
    }
    return 0;
}
