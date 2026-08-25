#pragma once

class CGContainerInfo {
public:
    static void EnterWorld();
    static void LeaveWorld();
    static void OpenContainer();
    static void UpdateContainers();
    static void UpdateContents();
    static void UpdateCooldowns();
    static void UpdateItem();
};

void ContainerRegisterScriptFunctions();
void ContainerUnregisterScriptFunctions();

