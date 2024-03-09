//cvars
enum
{
    DatabaseName,
    AnnounceGangCreate,
    AnnounceGangName,
    GangChat,
    MaxGangSize,
    GangTags,
    Version
}

ConVar cvarJBFS[Version+1];

enum
{
    RED=2,
    BLU
}

enum
{
    GangRank_None = 0,
    GangRank_Muscle,
    GangRank_Officer,
    GangRank_Boss
}
//match rank enum
char g_RankColors[4][32] = {
        "default",
        "silver",
        "arcana",
        "legendary"
}

//temporary variables which mimic the sql tables
//very useful to have these be separate!

//enum struct group!!!

enum struct GangData
{
    char name[32];
    char tag[5];
    bool hastag;
    int id;
    int num_online;
    int boss_promotion;
    int points;
    int shopbought;
    int shopactive;
    int alive_red;
    int alive_blue;
}

enum struct PlayerData
{
    int index;
    int mid;
    int rank;
    int invite;
    float gmenu_time;
}

GangData g_GangData[MAXPLAYERS+1];
PlayerData g_PlayerData[MAXPLAYERS+1];

Database hDatabase;

char GangTable[] = "Gangs"
char PlayerTable[] = "GangPlayers"

Menu g_GangMenu[MAXPLAYERS+1];
Menu g_ShopMenu[MAXPLAYERS+1];

//client menu defs
typedef ClientFilterFunc = function bool(int client);
typedef ClientRunFunc = function bool(int client,int target);
ClientRunFunc g_RunFn[MAXPLAYERS+1];
ClientFilterFunc g_filterFn[MAXPLAYERS+1];
Menu g_ClientMenu[MAXPLAYERS+1];

enum
{
    GangShop_None,
    GangShop_Banner,
    GangShop_Conch,
    GangShop_Backup
}

int g_ShopPoints[] = {
    0,
    2000,
    2000,
    2000
}

char g_ShopNames[4][32] = {
    "ShopItemNone",
    "ShopItemBanner",
    "ShopItemConch",
    "ShopItemBackup"
}