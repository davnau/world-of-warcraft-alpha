#pragma once

class CGDuelInfo {
public:
    void AcceptDuel();
    void CancelDuel();
    void InitializeGame();
    void OnDuelComplete();
    void OnDuelInBounds();
    void OnDuelOutOfBounds();
    void OnDuelRequested();
    void OnDuelWinner();
    void ShutdownGame();
    void StartDuel();
};


