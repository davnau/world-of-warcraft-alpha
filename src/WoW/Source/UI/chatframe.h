#pragma once

struct lua_State;
class CDataStore;

class CGChat {
public:
    void AddChannel();
    static void AddChatMessage(const char* text, int type, const char* player,
                               unsigned language, const char* channel,
                               const char* player2, const char* specialFlag);
    void AddTextEmoteMessage();
    void ChannelList();
    void ChannelNotify();
    static int ChatHandler(CDataStore* message);
    void CheckFlagChanged();
    void DisplayPendingUserList();
    void EnterWorld();
    void GetChannel();
    void GetChannelID();
    void GetChannelName();
    void GetChannelString();
    void GetPendingChatMessages();
    void HandleFlagsChanged();
    void HandleTextEmote();
    void InitializeGame();
    void LeaveWorld();
    void NameQueryCallback();
    void QueueChatText();
    void QueueTextEmote();
    void RemoveChannel();
    void ShutdownGame();
    void TextEmoteNameQueryCallback();
    void TranslateMessage();
    void UpdateLanguages();
};

int Script_SendChatMessage(lua_State* state);
int StringToChatType(const char* name, int* type);
int StringToLanguage(const char* name, unsigned* language);
void ChatRegisterScriptFunctions();
void ChatUnregisterScriptFunctions();
