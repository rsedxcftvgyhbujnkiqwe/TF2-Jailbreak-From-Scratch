enum
{
    RED=2,
    BLU
}

//general cvars
//always leave Version at the bottom
enum
{
    BalanceRatio=0,
    TextChannel,
    GuardCrits,
    RoundTime,
    WardayTime,
    MicBalance,
    MicWarden,
    FireVote,
    GuardFire,
    FireRatio,
    MarkerTime,
    MarkerRadius,
    MarkerColor,
    Version
}
ConVar cvarJBFS[Version+1];

//command cvars
//always leave ACMDWmenu at the bottom
enum
{
    ACMD_ForceWarden,
    ACMD_LockWarden,
    ACMD_JailTime,
    ACMD_Cells,
    ACMD_FF,
    ACMD_CC,
    ACMD_WardenMenu
}
ConVar cvarJBFS_ACMD[ACMD_WardenMenu+1];

enum
{
    W_Chose=0,
    W_Force,
    W_Random,
    UW_Retire=0,
    UW_Force,
    UW_Death,
    UW_Disconnect,
    UW_Fire,
    UW_TeamChange,
    UW_Silent
}

enum struct gamemode
{
    bool MicStatus[MAXPLAYERS+1];
    int Warden;
    bool WardenLocked;
    Handle WardenLockTimer;
    int TimeLeft;
    Handle RoundTimer;
    bool FireVotes[MAXPLAYERS+1];
    int FireVoteCount;
    bool FireStatus[MAXPLAYERS+1];
    bool MarkerPlaced;
    bool RoundActive;
    int AttachedParticle[MAXPLAYERS+1];
}
public gamemode JBFS;

enum struct JBMapConfig
{
    char cells[64];
    bool hascells;
    float fspawn[3];
    bool hasfspawn;
    float rspawn[3];
    bool hasrspawn;
    float bspawn[3];
    bool hasbspawn;
}
public JBMapConfig MapConfig;

enum
{
    LR_None,
    LR_FreedaySelf,
    LR_FreedayOthers,
    LR_FreedayAll,
    LR_Warday,
    LR_Custom,
    LR_Map
}

enum struct JBLRConfig
{
    int LRType;
    int LRWinner;
    Handle menu;
    int Freedays[3];
    char maplrs[32];
    int maplrcount;
    bool WardayActive;
    bool FreedaysActive[MAXCLIENTS+1];
}
public JBLRConfig LRConfig;

//jb events for vscript
enum 
{
    JBEvent_Warday,
    JBEvent_SetWarden,
    JBEvent_RemoveWarden
}

Menu g_WMenu[MAXPLAYERS+1];
float g_WMenuTime;
bool sourcecommspp = false;

enum struct Marker
{
    int BeamSprite;
    int GlowSprite;
    int HaloSprite;
    char MarkerColorHex[32];
    int MarkerColorRGBA[4];
}
public Marker JBMarker;

//client menu defs
typedef ClientFilterFunc = function bool(int client);
typedef ClientRunFunc = function void(int client,int target);
ClientRunFunc g_RunFn[MAXPLAYERS+1];
Menu g_ClientMenu[MAXPLAYERS+1];