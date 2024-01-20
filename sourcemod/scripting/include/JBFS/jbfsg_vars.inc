//cvars
enum
{
    DatabaseName,
    AnnounceGangCreate,
    AnnounceGangName,
    GangChat,
    MaxGangSize,
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
}

enum struct PlayerData
{
    int index;
    int mid;
    int rank;
    int invite;
}

GangData g_GangData[MAXPLAYERS+1];
PlayerData g_PlayerData[MAXPLAYERS+1];

Database hDatabase;

char GangTable[] = "Gangs"
char PlayerTable[] = "GangPlayers"