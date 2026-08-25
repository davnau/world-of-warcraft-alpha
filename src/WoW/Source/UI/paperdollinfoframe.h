#pragma once

class CGCharacterInfo {
public:
    static void EnterWorld();
    static void GetSkillInfoByIndex();
    static void GetSkillOffsetFromString();
    static void InitializeGame();
    static void InstallMirrorHandlers();
    static void LeaveWorld();
    static void OrderProficiencies();
    static void OrderSkillLines();
    static void PickupBag();
    static void PickupItem();
    static void PutItemInBackpack();
    static void PutItemInBag();
    static void RemoveMirrorHandlers();
    static void ShutdownGame();
    static void UpdateAllSkillLines();
    static void UpdateItem();
    static void UseItem();
};

void CharacterInfoRegisterScriptFunctions();
void CharacterInfoUnregisterScriptFunctions();

