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
    FreedayTime,
    MicBalance,
    MicWarden,
    FireVote,
    GuardFire,
    FireRatio,
    MarkerTime,
    MarkerRadius,
    MarkerColor,
    RemoveItem1,
    RemoveItem2,
    ReplaceWeapon,
    RedBuild,
    BlueBuild,
    RedBuildWarday,
    BlueBuildWarday,
    DroppedWeapon,
    DroppedAmmo,
    PointServerCMD,
    Version
}
ConVar cvarJBFS[Version+1];

enum 
{
    Building_Teleporter= (1<<0),
    Building_Dispenser= (1<<1),
    Building_Sentry= (1<<2)
}

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
    ACMD_ForceLR,
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
    int LRActive;
    int LRChosen;
    int LRWinner;
    Handle menu;
    int Freedays[3];
    char maplrs[32];
    int maplrcount;
    bool WardayActive;
    bool FreedaysActive[MAXPLAYERS+1];
    bool CustomChatWait;
    char CustomLR[64];
    bool LRPrinted;
}
public JBLRConfig LRConfig;

enum struct JBWeaponConfig
{
    int BlockedWeapons_Ammo[128];
    int BlockedWeapons_Warday[128];
    int BlockedWeapons_All[128];
}
public JBWeaponConfig WeaponConfig;
int g_BlockedWeapons[MAXPLAYERS+1][6];
bool g_HasBlockedWeapons[MAXPLAYERS+1];

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
bool vscript = false;

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
typedef ClientRunFunc = function bool(int client,int target);
ClientRunFunc g_RunFn[MAXPLAYERS+1];
ClientFilterFunc g_filterFn[MAXPLAYERS+1];
Menu g_ClientMenu[MAXPLAYERS+1];

//weapon defs
//this is for replacing removed weapons on a player

char g_WepName[3][10][32] = {
    {
        "tf_weapon_flamethrower",
        "tf_weapon_scattergun",
        "tf_weapon_sniperrifle",
        "tf_weapon_rocketlauncher",
        "tf_weapon_grenadelauncher",
        "tf_weapon_minigun",
        "tf_weapon_syringegun_medic",
        "tf_weapon_flamethrower",
        "tf_weapon_revolver",
        "tf_weapon_shotgun_primary"
    },
    {
        "tf_weapon_shotgun_pyro",
        "tf_weapon_pistol",
        "tf_weapon_smg",
        "tf_weapon_shotgun_soldier",
        "tf_weapon_pipebomblauncher",
        "tf_weapon_medigun",
        "tf_weapon_shotgun_hwg",
        "tf_weapon_shotgun_pyro",
        "tf_weapon_builder",
        "tf_weapon_pistol"
    },
    {
        "tf_weapon_fireaxe",
        "tf_weapon_bat",
        "tf_weapon_club",
        "tf_weapon_shovel",
        "tf_weapon_bottle",
        "tf_weapon_bonesaw",
        "tf_weapon_fists",
        "tf_weapon_fireaxe",
        "tf_weapon_knife",
        "tf_weapon_wrench"
    }
};

int g_WepIndex[3][10] = {
    {
        21,
        13,
        14,
        18,
        19,
        15,
        17,
        21,
        24,
        9
    },
    {
        12,
        23,
        16,
        10,
        20,
        29,
        11,
        12,
        735,
        22
    },
    {
        2,
        0,
        3,
        6,
        1,
        8,
        5,
        2,
        4,
        7
    }
};