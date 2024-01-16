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
    GangRank_Boss,
    GangRank_Officer,
    GangRank_Muscle
}
//match rank enum
char g_RankColors[4][32] = {
        "default",
        "legendary",
        "arcana",
        "silver"
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
    int mid;
    int rank;
    int invite;
}

GangData g_GangData[MAXPLAYERS+1];

Database hDatabase;

char GangTable[] = "Gangs"
char PlayerTable[] = "GangPlayers"