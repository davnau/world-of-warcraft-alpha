#pragma once

int CmdLineProcess();
int CmdLineProcess(const char* commandLine);
int CmdLineGetBool(unsigned id);
char* CmdLineGetString(unsigned id);
unsigned CmdLineGetUint(unsigned id);
