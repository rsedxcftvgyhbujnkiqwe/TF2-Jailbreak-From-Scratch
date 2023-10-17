//This is too much work right now
//maybe in another version...

#define PLUGIN_NAME         "[JBFS] Timeout"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Minimal teamban plugin for JBFS"

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

#include <jbfs>

enum
{
    ACMD_Timeout,
    Version
}
ConVar cvarJBFS[Version+1];

int g_GuardBanRounds[MAXPLAYERS+1];

enum
{
    Round_Pre,
    Round_Active,
    Round_Post
}
int RoundStatus = Round_Pre;

enum
{
    RED=2,
    BLU
}

enum
{
    Ban_Round,
    Ban_Map
}

Database hDatabase;

char DBName[] = "JBFSTimeouts"

public void OnPluginStart()
{
    cvarJBFS[DatabaseName] = CreateConVar("sm_jbfst_database","jbfstimeouts","Name of SQL database to use. Configured in databases.cfg",FCVAR_NOTIFY)
    cvarJBFS[ACMD_Timeout] = CreateConVar("sm_jbfst_timeout","2","Admin flag(s) required to timeout a player.",FCVAR_NOTIFY,true,0.0,true,2097151.0)
    AutoExecConfig(true,"JBFSTimeout");

    RegConsoleCmd("sm_timeoutstatus",Command_TimeoutStatus,"Check current timeout");
    RegAdminCmd("sm_timeout",Command_Admin_Timeout,cvarJBFS[ACMD_Timeout].IntValue,"Timeout a player from guard for a specified number of rounds");

    LoadTranslations("common.phrases");
    LoadTranslations("jbfs/jbfs.phrases");
    LoadTranslations("jbfs/jbfs.timeout");

    HookEvent("teamplay_round_start",OnPreRoundStart);
    HookEvent("arena_round_start",OnArenaRoundStart);
    HookEvent("teamplay_round_win",OnArenaRoundEnd);
    HookEvent("teamplay_round_stalemate",OnArenaRoundEnd);

    //connect to DB
    DBConnect();
}

public void DBConnect()
{
    char dbname[255];
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
    Format(query,sizeof(query), "CREATE TABLE IF NOT EXISTS %s"
        ... "(timestamp INT, "
		...	"offender_steamid VARCHAR(22), "
		... "offender_name VARCHAR(32), "
		... "admin_steamid VARCHAR(22), "
		... "admin_name VARCHAR(32), "
        ... "ban_type INT(16),"
		... "ban_length INT(16), "
		... "ban_left INT(16), "
		... "reason VARCHAR(200), "
		... "PRIMARY KEY (timestamp));",DBName);
    
    Transaction txn = new Transaction();

    tx.AddQuery(query);

    hDatabase.Execute(txn,TXN_OnSuccess,TXN_OnFailure);
}

public void DB_QueryCB(Database db, DBResultSet results, const char[] error, any data)
{
}

public void TXN_OnSuccess(Database db, any data, int numQueries, DBResultSet[] results, any[] queryData)
{
}

public void TXN_OnFailure(Database db, any data, int numQueries, DBResultSet[] results, any[] queryData)
{
}

public void DB_AddGuardBan(int client, int BanType, int duration, int admin, char[] reason)
{
    int ban_left = DB_GetGuardBanLeft(client);
    char ID[32]; GetClientAuthID(client,AuthId_Steam2,ID,sizeof(ID));
    char IDAdmin[32]; GetClientAuthID(admin,AuthId_Steam2,IDAdmin,sizeof(IDAdmin));

    if (ban_left > 0)
    {
        DB_UpdateBanLength(ID,0);
    }
    int timestamp = GetTime();
    char query[256];
    Format(query,sizeof(query),
            "INSERT INTO %s "
		... "(timestamp, offender_steamid, offender_name, admin_steamid, admin_name, ban_type, ban_length, ban_left) "
		... "VALUES (%d, '%s', '%N', '%s', '%N', %d, %d, '%s')",
			DBName, timestamp, ID, client, IDAdmin, admin, duration, duration, reason);
    
    hDatabase.Query(DB_QueryCB,query);

    JBFS_AddGuardBan(client);
}

public int DB_GetGuardBanLeft(int client)
{

    char ID[32]; GetClientAuthID(client,AuthId_Steam2,ID,sizeof(ID));

    char query[128];
    Format(query,sizeof(query), "SELECT ban_left"
        ... "FROM %s "
        ... "WHERE "
        ... "offender_steamid = '%s' "
        ... "AND ban_left > 0",DBName,ID);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null) return -1;

    int ban_left;
    while (SQL_FetchRow(query))
    {
        ban_left = SQL_FetchInt(hQuery, 7)
    }

    delete hQuery;

    if (ban_left == 0) return -1;

    return ban_left;
}

public void DB_UpdateBanLength(char[] ID, int duration)
{
    char query[256];
    Format(query,sizeof(query),
            "UPDATE %s"
        ... "SET ban_left = 0 "
        ... "WHERE offender_steamid = '%s' "
        ... "AND ban_left > 0 ",
        DBName,ID)
    hDatabase.Query(DB_QueryCB,query);
}

public bool DB_IsGuardBanned(int client)
{
    int left = DB_GetGuardBanLeft(client);
    if (left > 0) return true;
    else return false;
}

public Action Command_Admin_Timeout(int client, int args)
{
    if (args < 2)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandTimeoutUsage")
        return Plugin_Handled;
    }
    char arg1[32],arg2[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int target = FindTarget(client, arg1);
    if (target == -1) return Plugin_Handled;

    GetCmdArg(2, arg2, sizeof(arg2));
    int rounds = StringToInt(arg2);
    if (rounds < 1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandTimeoutUsage")
        return Plugin_Handled;
    }

	char reason[256];

	for (int i = 3; i <= args; i++)
	{
		GetCmdArg(i, s, sizeof(s));
		Format(reason, sizeof(reason), "%s %s", reason, s);
	}

    DB_AddGuardBan(target,Ban_Round,rounds,client,reason)

    //pre round -> will be moved
    switch(RoundStatus)
    {
        case Round_Pre, Round_Active:
        {
            if(GetClientTeam(target) == BLU) ForcePlayerSuicide(target);
        }
    }
    CShowActivity2(client, "{day9}[JBFS]", "{unique}%N{wheat} has been timed out from guard for %d rounds!",target,rounds);
    return Plugin_Handled;
}

public Action Command_Admin_RemoveTimeout(int client, int args)
{
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandRemoveTimeoutUsage")
        return Plugin_Handled;
    }
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int target = FindTarget(client, arg1);
    if (target == -1) return Plugin_Handled;

    if (DB_IsGuardBanned(target))
    {
        char ID[32]; GetClientAuthID(i,AuthId_Steam2,ID,sizeof(ID));
        DB_UpdateBanLength(ID,0);
    }
    return Plugin_Handled;
}

public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    RoundStatus = Round_Pre;
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    RoundStatus = Round_Active;
}

public Action OnArenaRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    RoundStatus = Round_Post;
    //end of round - lower ban length by 1 for clients in server
    //ban is not reduced unless client is present!!
    for (int i=1;i<=MaxClients;i++)
    {
        int length = DB_GetGuardBanLeft(i);
        if (length > 0)
        {
            char ID[32]; GetClientAuthID(i,AuthId_Steam2,ID,sizeof(ID));
            DB_UpdateBanLength(ID,length--)
            //ban just ran out
            if (length == 0) JBFS_RemoveGuardBan(i);
        }
    }
}

public void OnClientPutInServer(client)
{
    if (DB_IsGuardBanned(client))
    {
        JBFS_AddGuardBan(client);
    }
}