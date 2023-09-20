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

enum celldoortype
{
    Door,
    Breakable,
    Relay
}

enum struct JBMapConfig
{
    char cells[32];
    bool hascells;
    int fspawn[3];
    bool hasfspawn;
    int rspawn[3];
    bool hasrspawn;
    int bspawn[3];
    bool hasbspawn;
}
public JBMapConfig MapConfig;