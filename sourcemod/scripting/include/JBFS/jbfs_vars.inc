enum
{
    RED=2,
    BLU
}

enum
{
    BalanceRatio=0,
    TextChannel,
    GuardCrits,
    RoundTime,
    Version
}
ConVar cvarJBFS[Version+1];

enum
{
    W_Chose=0,
    W_Force,
    W_Random,
    UW_Retire=0,
    UW_Force,
    UW_Death,
    UW_Disconnect
}

enum struct gamemode
{
    bool HasMic[MAXPLAYERS+1];
    int Warden;
    bool WardenLocked;
    Handle WardenLockTimer;
    int TimeLeft;
    Handle RoundTimer;
}
public gamemode JBFS;

enum struct JBMapConfig
{
    char cells[PLATFORM_MAX_PATH];
    bool hascells;
    float fspawn[3];
    bool hasfspawn;
    float rspawn[3];
    bool hasrspawn;
    float bspawn[3];
    bool hasbspawn;
}
public JBMapConfig MapConfig;