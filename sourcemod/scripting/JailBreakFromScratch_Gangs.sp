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
    AnnounceGangCreate,
    AnnounceGangName,
    GangChat,
    Version
}

enum
{
    RED=2,
    BLU
}

enum
{
    GangRank_None = -1,
    GangRank_Boss = 0,
    GangRank_Officer,
    GangRank_Muscle
}
//match rank enum
g_RankColors = {
        "default",
        "axis",
        "darkslateblue",
        "darkslategray"
}

//temporary variables which mimic the sql tables
//very useful to have these be separate!

//enum struct group!!!

enum struct GangData
{
    int uid = -1;
    int rank = GangRank_None;
    char name[32];
    char tag[5];
    bool hastag;
}

GangData g_GangData[MAXPLAYERS+1];

public void OnPluginStart()
{
    cvarJBFS[DatabaseName] = CreateConVar("sm_jbfsg_database","jbfsgangs","Name of SQL database to use. Configured in databases.cfg",FCVAR_NOTIFY)
    cvarJBFS[AnnounceGangCreate] = CreateConVar("sm_jbfsg_announcegangcreate","1","Whether to announce to all players when gangs are created.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[AnnounceGangName] = CreateConVar("sm_jbfsg_announcegangname","1","Whether to announce to all players when gangs change their names.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[GangChat] = CreateConVar("sm_jbfsg_gangchat","1","Enable the Gang chat feature, which allows gang members to send messages only other gang members can see.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[Version] = CreateConVar("jbfst_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);

    //generic cmds
    RegConsoleCmd("sm_ganghelp",Command_GangHelp,"Help menu for Gang related commands")
    RegConsoleCmd("sm_creategang",Command_CreateGang,"Create a gang with the specified name.");

    //gang cmds
    RegConsoleCmd("sm_gangname",Command_SetGangName,"Change the name of your gang.");
    RegConsoleCmd("sm_gangtag",Command_SetGangTag,"Change your gang's tag.");
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
        ... "nid VARCHAR(32), " //name ID - string that represents simplified gang name. 
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
            ... "AS \"InGang\"",PlayerTable,ID,gang_uid);
    }
    else
    {
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE steamid = '%s' "
            ... "AND gang_uid != -1) "
            ... "AS \"InGang\"",PlayerTable,ID,gang_uid);
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
        ... "WHERE steamid = '%s' ",PlayerTable,ID,gang_uid);

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

//to reduce sql overhead, it will be assumed that this is only ever called IF a player is in a gang
public int DB_GetPlayerGangRank(int client)
{
    char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));
    char query[256];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
            "SELECT rank "
        ... "FROM %s "
        ... "WHERE steamid = '%s' ",PlayerTable,ID,gang_uid);

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
    */
    int rank = -1;
    while (SQL_FetchRow(hQuery))
    {
        rank = SQL_FetchInt(hQuery, 0);
        if (rank >= 0) break;
    }
    delete hQuery;
    return rank;
}

public bool DB_SetPlayerGang(int client, int gang_uid, int rank)
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
            ... "rank = %d "
            ... "WHERE steamid = '%s' ",
            PlayerTable,client,gang_uid,rank,ID)
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
                PlayerTable, ID, client, gang_uid, rank);
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

public bool DB_GetGangName(int gang_uid, char[] buffer, int maxlength)
{
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT name "
            ... "FROM %s "
            ... "WHERE uid = %d ",GangTable,gang_uid);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    } 

    while (SQL_FetchRow(hQuery))
    {
        SQL_FetchString(hQuery, 0, buffer, maxlength);
    }
    delete hQuery;

    if (StrCompare(buffer,"\0")) return false;
    return true;
}

public bool DB_GetGangTag(int gang_uid, char[] buffer, int maxlength)
{
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT tag "
            ... "FROM %s "
            ... "WHERE uid = %d ",GangTable,gang_uid);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    } 

    while (SQL_FetchRow(hQuery))
    {
        SQL_FetchString(hQuery, 0, buffer, maxlength);
    }
    delete hQuery;

    if (StrCompare(buffer,"\0")) return false;
    return true;
}

//we assume gang exists for this
//check with DB_GangExists!!!
public bool DB_SetGangName(int gang_uid, char name[32])
{
    char query[128];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
        "UPDATE %s "
    ... "SET name = '%s', "
    ... "WHERE uid = %d ",
    GangTable,name,gang_uid)
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }
    return true;
}

/*
    does not check if a gang already exists with this name!
    this is purely for creation!
    make the check before you call it!!
*/
public int DB_CreateGang(char name[32])
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
        return -1;
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

    char nid[sizeof(name)];
    ReduceString(nid,sizeof(name),name);

    //create the new gang
     SQL_FormatQuery(hDatabase,query,sizeof(query),
                "INSERT INTO %s "
            ... "(name, tag, uid, nid, members, status) "
            ... "VALUES ('%s', '%s', %d, %d, %d)",
                GangTable, ID, "\0", uid, nid, 0, 0);
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return -1;
    }
    return uid;
}

public bool DB_GangExists(int gang_uid)
{
    if(gang_uid == -1) return false;

    char query[128];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE uid = %d) "
            ... "AS \"GangExists\"",GangTable,gang_uid);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    } 

    bool GangExists;
    while (SQL_FetchRow(hQuery))
    {
        GangExists = SQL_FetchInt(hQuery, 0);
        if (GangExists) break;
    }
    delete hQuery;

    return GangExists;
}

public bool DB_GangNameExists(char name[32])
{
    char nid[sizeof(name)];
    ReduceString(nid,sizeof(name),name);

    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE nid = '%s') "
            ... "AS \"GangExists\"",GangTable,nid);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    } 

    bool GangExists;
    while (SQL_FetchRow(hQuery))
    {
        GangExists = SQL_FetchInt(hQuery, 0);
        if (GangExists) break;
    }
    delete hQuery;

    return GangExists;
}

public bool DB_GangTagExists(char tag[5])
{
    char reducetag[sizeof(tag)];
    ReduceString(reducetag,sizeof(tag),tag);

    char query[128];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE tag = '%s') "
            ... "AS \"TagExists\"",GangTable,reducetag);
    
    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    } 

    bool TagExists;
    while (SQL_FetchRow(hQuery))
    {
        TagExists = SQL_FetchInt(hQuery, 0);
        if (TagExists) break;
    }
    delete hQuery;

    return TagExists;
}

public void ReduceString(char[] buffer, int maxlength, const char[] source)
{
    int bufferpos;
    for (int i;i<maxlength;i++)
    {
        char current = source[i];
        if (current >= 'A' && current <= 'Z')
            current += 32;
        if (char >= 'a' && char <= 'z')
            buffer[bufferpos++] = current;
    }
    buffer[bufferpos] = '\0';
}

public bool ValidAlphaString(char[] string, int maxlength)
{
    bool IsValid = true;
    int charcount;
    for(int i;i<maxlength;i++)
    {
        char current = string[i];
        if((current >= 'A' && current <= 'Z') || (current >= 'a' && current <= 'z'))
        {
            charcount++;
            continue;
        }
        else if(current == '\0') break;
        IsValid = false;
        break;
    }
    //address situation of empty string
    if (charcount == 0) IsValid = false;
    return IsValid;
}

public Action Command_CreateGang(int client, int args)
{
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandCreateGangUsage");
        CPrintToChat(client,"%t %t","PluginTag","GangNameFormat");
        CPrintToChat(client,"%t %t","PluginTag","GangNameExample");
        return Plugin_Handled;
    }

    if(client < 1){
        PrintToServer("[JBFS] You cannot create gangs from console.")
        return Plugin_Handled;
    }
    
    if (IsPlayerInGang(client,-1)){
        CPrintToChat(client,"%t %t","PluginTag","CommandCreateGangWarnInGang")
        return Plugin_Handled;
    }

    char gangname[32], s[32];
    for (int i = 1; i <= args; i++)
    {
        GetCmdArg(i, s, sizeof(s));
        if (StrEqual(gangname,"\0")) Format(gangname,sizeof(gangname),"%s%s",gangname,s)
        else Format(gangname, sizeof(gangname), "%s %s", gangname, s);
    }

    if (DB_GangNameExists(gangname))
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandCreateGangWarnExists");
        CPrintToChat(client,"%t %t","PluginTag","GangNameFormat");
        CPrintToChat(client,"%t %t","PluginTag","GangNameExample");
        return Plugin_Handled;
    }

    /*
     * by now we have checked that:
     * 1. a gang name is provided
     * 2. player is able to create a gang
     * 3. player is not in a gang
     * 4. a gang by the name doesn't exist already
     * so we should be able to safely create the gang
    */
    int gang_uid = DB_CreateGang(gangname);
    if (gang_uid == -1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandCreateGangError")
        return Plugin_Handled;
    }

    //this handles all the player setting - no need to be redundant
    SetPlayerGang(client,gang_uid,GangRank_Boss,true);

    if(cvarJBFS[AnnounceGangCreate].BoolValue)
    {
        CPrintToChatAll("%t %t","PluginTag","GangCreated",client,gangname);
    }
    else
    {
        CPrintToChat(client,"%t %t","GangTag","GangCreatedSilent",gangname);
    }
    CPrintToChat(client,"%t %t","PluginTag","GangCreatedHelp");
}

public Action Command_SetGangTag(int client, int args)
{
    if (g_GangData[client].uid == -1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnNotInGang")
        return Plugin_Handled;
    }
    if (g_GangData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    if (args != 1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSetNameUsage");
        CPrintToChat(client,"%t %t","PluginTag","GangTagFormat");
        CPrintToChat(client,"%t %t","PluginTag","GangTagExample");
        return Plugin_Handled;
    }
    //player is in a gang, and is the boss

    /*  gang tag is enforced differently than gang name
        tag must be:
        - 2-4 characters long
        - only contain letters
        I will not bother enforcing tags > 4, the buffer should handle it I think and auto truncate
    */
    char tag[5]
    GetCmdArg(1, tag, sizeof(tag));
    if (!ValidAlphaString(tag))
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSetNameUsage");
        CPrintToChat(client,"%t %t","PluginTag","GangTagFormat");
        CPrintToChat(client,"%t %t","PluginTag","GangTagExample");
        return Plugin_Handled;
    }

    int gang_uid = g_GangData[client].uid;

    if(SetGangTag(gang_uid,tag,true))
        CPrintToChatGang(gang_uid,"%t %t","GangTag","GangTagChange",tag);
}

public Action Command_SetGangName(int client, int args)
{
    if (g_GangData[client].uid == -1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnNotInGang")
        return Plugin_Handled;
    }
    if (g_GangData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSetNameUsage");
        return Plugin_Handled;
    }
    //player is in a gang, and is the boss
    char gangname[32], s[32];
    for (int i = 1; i <= args; i++)
    {
        GetCmdArg(i, s, sizeof(s));
        if (StrEqual(gangname,"\0")) Format(gangname,sizeof(gangname),"%s%s",gangname,s)
        else Format(gangname, sizeof(gangname), "%s %s", gangname, s);
    }

    int gang_uid = g_GangData[client].uid;

    if(SetGangName(gang_uid,gangname,true))
    {
        if(varJBFS[AnnounceGangName].BoolValue)
            CPrintToChatAll("%t %t","PluginTag","GangNameChange",current_name,new_name);
        else
            CPrintToChatGang(gang_uid,"%t %t","GangTag","GangNameChangeSilent");
    }
}

public Action Command_GangChat(int client, int args)
{
    if (!cvarJBFS[GangChat].BoolValue)
    {
        CPrintToChat(client,"%t %t","PluginTag","GangChatDisabled");
        return Plugin_Handled;
    }
    if(g_GangData[client].uid == -1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }

    char message[255], s[32];
    for (int i = 1; i <= args; i++)
    {
        GetCmdArg(i, s, sizeof(s));
        if (StrEqual(message,"\0")) Format(message,sizeof(message),"%s",s)
        else Format(message, sizeof(message), "%s %s", message, s);
    }

    CPrintToChatGang(g_GangData[client].uid,"%t {%s}%N{default}: %s","GangTag",g_RankColors[client],client,message);
}

public void OnClientPutInServer(int client)
{
    int gang_uid = DB_GetPlayerGang(client);
    int rank = GangRank_None;
    if (gang_uid >= 0) rank = DB_GetPlayerGangRank(client);
    SetPlayerGang(client,gang_uid,rank,false);
}

public void OnClientDisconnect(int client)
{
    SetPlayerGang(client,-1,GangRank_None,false);
}

public bool IsPlayerInGang(int client, int gang_uid)
{
    if (gang_uid >= 0)
    {
        return g_GangData[client].uid == gang_uid;
    }
    else
    {
        return g_GangData[client].uid > -1;
    }
}

public void SetPlayerGang(int client, int gang_uid, int gang_rank, bool savedatabase = true)
{
    g_GangData[client].uid = gang_uid;
    g_GangData[client].rank = gang_rank;

    DB_GetGangName(gang_uid,g_GangData[client].name,sizeof(g_GangData[client].name));
    if(DB_GetGangTag(gang_uid,g_GangData[client].tag,sizeof(g_GangData[client].tag)))
        g_GangData[client].hastag = true;
    else
        g_GangData[client].hastag = false;

    if(savedatabase) DB_SetPlayerGang(client,gang_uid,gang_rank);
}

public bool SetGangName(int gang_uid, char new_name[32], bool savedatabase = true)
{
    char current_name[32];
    DB_GetGangName(gang_uid,current_name,sizeof(current_name));

    if(savedatabase) DB_SetGangName(gang_uid,new_name);
    for(int i=1;i<=MaxPlayers;i++)
    {
        if (g_GangData[client].uid == gang_uid)
            g_GangData[client].name = new_name;
    }
    return true;
}

public bool SetGangTag(int gang_uid, char new_tag[5], bool savedatabase = true)
{
    char current_tag[5];
    DB_GetGangTag(gang_uid,current_tag,sizeof(current_tag))
    for(int i=1; i<MaxPlayers;i++)
    {
        if (g_GangData[client].uid == gang_uid)
            g_GangData[client].tag = new_tag;
    }
    return true;
}

public void ResetClientGangData(int client)
{
    g_GangData[client].uid = -1;
    g_GangData[client].rank = GangRank_None;
    g_GangData[client].name = "\0"
    g_GangData[client].tag = "\0"
    g_GangData[client].hastag = false;
}

stock CPrintToChatGang(int gang_uid, const String:message[], any:...) {
	CCheckTrie();
	decl String:buffer[MAX_BUFFER_LENGTH], String:buffer2[MAX_BUFFER_LENGTH];
	for(int i = 1; i <= MaxClients; i++) {
		if(!IsClientInGame(i) || g_GangData[i].uid != gang_uid) {
			continue;
		}
		Format(buffer, sizeof(buffer), "\x01%s", message);
		VFormat(buffer2, sizeof(buffer2), buffer, 2);
		CReplaceColorCodes(buffer2);
		CSendMessage(i, buffer2);
	}
}