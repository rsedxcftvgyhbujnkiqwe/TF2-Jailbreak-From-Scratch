#define PLUGIN_NAME         "[JBFS] Gangs"
#define PLUGIN_VERSION      "1.0"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Gang plugin for JBFS"

#include <sourcemod>
#include <sdktools>

public Plugin myinfo =
{
	name = PLUGIN_NAME,
	author = PLUGIN_AUTHOR,
	description = PLUGIN_DESCRIPTION,
	version = PLUGIN_VERSION,
	url = "https://github.com/rsedxcftvgyhbujnkiqwe/TF2-Jailbreak-From-Scratch"
};

#include <morecolors>

Database hDatabase;

char GangTable[] = "Gangs"
char PlayerTable[] = "GangPlayers"

//cvars
enum
{
    DatabaseName,
    Version
}

enum
{
    RED=2,
    BLU
}

enum
{
    RankBoss,
    RankMuscle
}

//temporary variables which mimic the sql tables
//very useful to have these be separate!

int g_PlayerGang[MAXPLAYERS+1] = { -1, ... }
int g_PlayerGangRank[MAXPLAYERS+1] = { -1, ... }

public void OnPluginStart()
{
    cvarJBFS[DatabaseName] = CreateConVar("sm_jbfsg_database","jbfsgangs","Name of SQL database to use. Configured in databases.cfg",FCVAR_NOTIFY)
    cvarJBFS[Version] = CreateConVar("jbfst_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);
}

public void DBConnect()
{
    char dbname[256];
    cvarJBFS[DatabaseName].GetString(dbname,sizeof(dbname))
    Database.Connect(GotDatabase,dbname)
}

public void GotDatabase(Database db, const char[] error, any data)
{
    if (db == null)
    {
        LogError("Database failure: %s",error);
    }
    else
    {
        hDatabase = db;
    }

    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query), "CREATE TABLE IF NOT EXISTS %s"
        ... "(name VARCHAR(32), " //name of gang
		...	"tag VARCHAR(4), " //gang tag, 4 chars, abbreviation
		... "uid INT, " //uid, unique to every gang
		... "members INT, " //# of members
        ... "status INT, " //status - 1 = active, 0 = disbanded
		... "PRIMARY KEY (gang_id));",GangTable);
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return;
    }

    SQL_FormatQuery(hDatabase,query,sizeof(query), "CREATE TABLE IF NOT EXISTS %s"
        ... "(name VARCHAR(32), "
		...	"steamid VARCHAR(22), "
		... "gang_uid INT, "
		... "rank INT, "
		... "PRIMARY KEY (player_steamid));",PlayerTable);
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return;
    }
}

public bool DB_IsPlayerInGang(int client, int gang_uid)
{
    char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));

    char query[256];
    //pass -1 to check if in any gang, otherwise pass uid
    if (gang_uid >= 0)
    {
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE steamid = '%s' "
            ... "AND gang_uid = %d) "
            ... "AS \"InGang\"",GangPlayers,ID,gang_uid);
    }
    else
    {
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE steamid = '%s') "
            ... "AS \"InGang\"",GangPlayers,ID,gang_uid);
    }

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return -1;
    } 

    bool InGang;
    while (SQL_FetchRow(hQuery))
    {
        InGang = SQL_FetchInt(hQuery, 0);
        if (InGang) break;
    }
    delete hQuery;
    return InGang;
}

public int DB_GetPlayerGang(int client)
{
    char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));
    char query[256];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
            "SELECT gang_uid "
        ... "FROM %s "
        ... "WHERE steamid = '%s' ",GangPlayers,ID,gang_uid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return -1;
    } 

    /*
       there should only ever be one record for every player
       break after first, in the unlikely event that a player
       is in two gangs at once, first takes priority
       however, DB_IsPlayerInGang still returns true for all gang(s) a player is in!
    */
    int gang_uid = -1;
    while (SQL_FetchRow(hQuery))
    {
        gang_uid = SQL_FetchInt(hQuery, 0);
        if (gang_uid >= 0) break;
    }
    delete hQuery;
    return gang_uid;
}

public bool DB_SetPlayerGang(int client,int gang_uid)
{
    char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));

    //this check is annoying but luckily it only ever needs to be made here
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE steamid = '%s') "
            ... "AS \"InGang\"",PlayerTable,ID);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return -1;
    } 

    bool InGang;
    while (SQL_FetchRow(hQuery))
    {
        InGang = SQL_FetchInt(hQuery, 0);
        if (InGang) break;
    }
    delete hQuery;


    if (InGang)
    {
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                "UPDATE %s "
            ... "SET name = %N, "
            ... "gang_uid = %d, "
            ... "rank = 1 "
            ... "WHERE steamid = '%s' ",
            PlayerTable,client,gang_uid,ID)
        if (!SQL_FastQuery(hDatabase, query))
        {
            char qerror[255];
            SQL_GetError(hDatabase, qerror, sizeof(qerror));
            PrintToServer("Failed to query (error: %s)", qerror);
            return false;
        }
    }
    else
    {
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                "INSERT INTO %s "
            ... "(steamid, name, gang_uid, rank) "
            ... "VALUES ('%s', '%N', '%d', '%d')",
                PlayerTable, ID, client, gang_uid, 1);
        if (!SQL_FastQuery(hDatabase, query))
        {
            char qerror[255];
            SQL_GetError(hDatabase, qerror, sizeof(qerror));
            PrintToServer("Failed to query (error: %s)", qerror);
            return false;
        }
    }
    return true;
}

/*
    does not check if a gang already exists with this name!
    this is purely for creation!
*/
public bool DB_CreateGang(char name[32])
{
    //get current largest uid
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT MAX(gang_uid) "
            ... "AS largest_uid "
            ... "FROM %s",GangTable);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    } 

    int uid;
    while (SQL_FetchRow(hQuery))
    {
        uid = SQL_FetchInt(hQuery, 0);
    }
    delete hQuery;

    //at this point we have the largest uid
    //therefore, increase it to get the new uid for this new gang
    uid++;

    //create the new gang
     SQL_FormatQuery(hDatabase,query,sizeof(query),
                "INSERT INTO %s "
            ... "(name, tag, uid, members, status) "
            ... "VALUES ('%s', '%s', %d, %d, %d)",
                GangTable, ID, "\0", uid, 0, 0);
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }
    return true;
}

public bool DB_GangExists(char name[32])
{
    
}