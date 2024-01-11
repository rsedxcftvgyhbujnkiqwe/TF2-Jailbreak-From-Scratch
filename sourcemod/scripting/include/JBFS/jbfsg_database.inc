stock void DBConnect()
{
    char dbname[256];
    cvarJBFS[DatabaseName].GetString(dbname,sizeof(dbname))
    Database.Connect(GotDatabase,dbname)
}

stock void GotDatabase(Database db, const char[] error, any data)
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
        ... "nid VARCHAR(32), " //name ID, string that represents simplified gang name.
        ... "status INT, " //status, 1 = active, 0 = disbanded
        ... "creation_date INT, " //unix timestamp
		... "PRIMARY KEY (uid));",GangTable);
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
        ... "mid INT, "
		... "PRIMARY KEY (steamid));",PlayerTable);
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return;
    }
}

stock int DB_GetPlayerGang(int client)
{
    char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));
    char query[256];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
            "SELECT gang_uid "
        ... "FROM %s "
        ... "WHERE steamid = '%s' ",PlayerTable,ID);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return 0;
    }

    /*
       there should only ever be one record for every player
       break after first, in the unlikely event that a player
       is in two gangs at once, first takes priority
       however, DB_IsPlayerInGang still returns true for all gang(s) a player is in!
    */
    int gang_uid;
    while (SQL_FetchRow(hQuery))
    {
        gang_uid = SQL_FetchInt(hQuery, 0);
        if (gang_uid > 0) break;
    }
    delete hQuery;
    return gang_uid;
}

//to reduce sql overhead, it will be assumed that this is only ever called IF a player is in a gang
stock int DB_GetPlayerGangRank(int client)
{
    char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));
    char query[256];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
            "SELECT rank "
        ... "FROM %s "
        ... "WHERE steamid = '%s' ",PlayerTable,ID);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return 0;
    }

    /*
       there should only ever be one record for every player
       break after first, in the unlikely event that a player
       is in two gangs at once, first takes priority
    */
    int rank = 0;
    while (SQL_FetchRow(hQuery))
    {
        rank = SQL_FetchInt(hQuery, 0);
        if (rank > 0) break;
    }
    delete hQuery;
    return rank;
}

//assuming they are in a gang!!!
stock bool DB_SetPlayerGangRankByMID(int gang_uid, int mid, int rank)
{
    SQL_FormatQuery(hDatabase,query,sizeof(query),
            "UPDATE %s "
        ... "SET rank = %d "
        ... "WHERE mid = %d "
        ... "AND gang_uid = %d",PlayerTable,rank,mid,g_GangData[client].id)
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }
    return true;
}

stock bool DB_SetPlayerGang(int client, int gang_uid, int rank, int mid)
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
        return false;
    }

    int InGang;
    while (SQL_FetchRow(hQuery))
    {
        InGang = SQL_FetchInt(hQuery, 0);
        if (InGang) break;
    }
    delete hQuery;

    char name[32], char sanitized_name[sizeof(name)];
    Format(name,sizeof(name),"%N",client);
    SanitizeString(sanitized_name,sizeof(name),name);

    if (InGang)
    {
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                "UPDATE %s "
            ... "SET name = %s, "
            ... "gang_uid = %d, "
            ... "rank = %d, "
            ... "mid = %d "
            ... "WHERE steamid = '%s' ",
            PlayerTable,sanitized_name,gang_uid,rank,mid,ID)
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
            ... "VALUES ('%s', '%s', '%d', '%d')",
                PlayerTable, ID, sanitized_name, gang_uid, rank);
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

stock bool DB_GetGangName(int gang_uid, char[] buffer, int maxlength)
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

    if (strcmp(buffer,"\0")==0) return false;
    return true;
}

stock bool DB_GetGangTag(int gang_uid, char[] buffer, int maxlength)
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

    if (strcmp(buffer,"\0")==0) return false;
    return true;
}

//we assume gang exists for this
//check with DB_GangExists!!!
stock bool DB_SetGangName(int gang_uid, char name[32])
{
    char nid[sizeof(name)];
    ReduceString(nid,sizeof(name),name);
    char query[128];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
        "UPDATE %s "
    ... "SET name = '%s', "
    ... "nid = '%s', "
    ... "WHERE uid = %d ",
    GangTable,name,nid,gang_uid)
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }
    return true;
}

stock bool DB_SetGangTag(int gang_uid, char tag[5])
{
    char query[128];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
        "UPDATE %s "
    ... "SET tag = '%s', "
    ... "WHERE uid = %d ",
    GangTable,tag,gang_uid)
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
stock int DB_CreateGang(char name[32])
{
    //get current largest uid
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT MAX(uid) "
            ... "AS largest_uid "
            ... "FROM %s",GangTable);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return 0;
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
        ... "(name, tag, uid, nid, status, creation_date) "
        ... "VALUES ('%s', '%s', %d, %s, %d, %d)",
            GangTable, name, "\0", uid, nid, 0, GetTime());
    if (!SQL_FastQuery(hDatabase, query))
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return 0;
    }
    return uid;
}

stock bool DB_GangExists(int gang_uid)
{
    if(gang_uid == 0) return false;

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

    int GangExists;
    while (SQL_FetchRow(hQuery))
    {
        GangExists = SQL_FetchInt(hQuery, 0);
        if (GangExists) break;
    }
    delete hQuery;

    return view_as<bool>(GangExists);
}

stock bool DB_GangNameExists(char name[32])
{
    char nid[sizeof(name)];
    ReduceString(nid,sizeof(name),name);

    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE nid = '%s' "
            ... "AND status = 1 "
            ... "AS \"GangExists\"",GangTable,nid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }

    int GangExists;
    while (SQL_FetchRow(hQuery))
    {
        GangExists = SQL_FetchInt(hQuery, 0);
        if (GangExists) break;
    }
    delete hQuery;

    return view_as<bool>(GangExists);
}

stock bool DB_GangTagExists(char tag[5])
{
    char query[128];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT exists("
            ... "SELECT * FROM %s "
            ... "WHERE tag = '%s') "
            ... "AS \"TagExists\"",GangTable,tag);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }

    int TagExists;
    while (SQL_FetchRow(hQuery))
    {
        TagExists = SQL_FetchInt(hQuery, 0);
        if (TagExists) break;
    }
    delete hQuery;

    return view_as<bool>(TagExists);
}

stock int DB_GetGangSize(int gang_uid)
{
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT COUNT(steamid) "
            ... "FROM %s "
            ... "WHERE gang_uid = %d ",PlayerTable,gang_uid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return 0;
    }

    int members = 0;
    while (SQL_FetchRow(hQuery))
    {
        members = SQL_FetchInt(hQuery, 0);
        if (members >= 0) break;
    }
    delete hQuery;

    return members;
}

stock void DB_RemovePlayerFromGang(int client, int gang_uid)
{
    if (gang_uid > 0)
    {
        char ID[32]; GetClientAuthId(client,AuthId_Steam2,ID,sizeof(ID));
        char query[512];
        SQL_FormatQuery(hDatabase,query,sizeof(query), 
                "DELETE FROM %s "
            ... "WHERE gang_uid = %d, "
            ... "AND steamid = '%s'",PlayerTable,gang_uid,ID);
        if (!SQL_FastQuery(hDatabase, query))
        {
            char qerror[255];
            SQL_GetError(hDatabase, qerror, sizeof(qerror));
            PrintToServer("Failed to query (error: %s)", qerror);
            return;
        }
    }
}
stock void DB_RemovePlayerFromGangByMID(int gang_uid, int mid)
{
    if (gang_uid > 0)
    {
        char query[512];
        SQL_FormatQuery(hDatabase,query,sizeof(query), 
                "DELETE FROM %s "
            ... "WHERE gang_uid = %d, "
            ... "AND mid = %d",PlayerTable,gang_uid,mid);
        if (!SQL_FastQuery(hDatabase, query))
        {
            char qerror[255];
            SQL_GetError(hDatabase, qerror, sizeof(qerror));
            PrintToServer("Failed to query (error: %s)", qerror);
            return Plugin_Handled;
        }
    }
}

stock void DB_SetGangStatus(int gang_uid, int status)
{
    if (gang_uid > 0)
    {
        char query[512];
        SQL_FormatQuery(hDatabase,query,sizeof(query), 
                "UPDATE %s "
            ... "SET status = %d "
            ... "WHERE gang_uid = %d",PlayerTable,status,gang_uid);
        if (!SQL_FastQuery(hDatabase, query))
        {
            char qerror[255];
            SQL_GetError(hDatabase, qerror, sizeof(qerror));
            PrintToServer("Failed to query (error: %s)", qerror);
            return;
        }
    }
}

stock bool DB_IsGangActive(int gang_uid)
{
    if (gang_uid > 0)
    {
        char query[512];
        SQL_FormatQuery(hDatabase,query,sizeof(query),
                    "SELECT status "
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

        int status = -1;
        while (SQL_FetchRow(hQuery))
        {
            status = SQL_FetchInt(hQuery, 0);
            if (status >= 0) break;
        }
        delete hQuery;

        if (status == 1) return true;
    }
    return false
}

stock int DB_GetMaxMID(int gang_uid)
{
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT MAX(mid) "
            ... "AS largest_mid "
            ... "FROM %s "
            ... "WHERE gang_uid = %d",PlayerTable,gang_uid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return 0;
    }

    int mid;
    while (SQL_FetchRow(hQuery))
    {
        mid = SQL_FetchInt(hQuery, 0);
    }
    delete hQuery;
    return mid;
}

stock bool DB_MIDExists(int gang_uid, int mid)
{
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT COUNT(steamid) "
            ... "FROM %s "
            ... "WHERE gang_uid = %d "
            ... "AND mid = %d",PlayerTable,gang_uid,mid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }

    int count;
    while (SQL_FetchRow(hQuery))
    {
        count = SQL_FetchInt(hQuery, 0);
        if (count >= 0) break;
    }

    delete hQuery;
    return (count > 0);
}

//no two players in one gang should share a member ID
stock bool DB_GetInfoByMID(int gang_uid, int mid, char[32] id_buffer, char[32] name_buffer, int &rank_buffer)
{
    //get relevant player
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT steamid, name, rank "
            ... "FROM %s "
            ... "WHERE gang_uid = %d "
            ... "AND mid = %d",PlayerTable,gang_uid,mid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return false;
    }
    bool fetched = false;
    while (SQL_FetchRow(hQuery))
    {
        SQL_FetchString(hQuery, 0, id_buffer, sizeof(id_buffer));
        SQL_FetchString(hQuery, 1, name_buffer, sizeof(name_buffer));
        rank_buffer = SQL_FetchInt(hQuery, 2);
        if (!StrEqual(ID,"\0")) 
        {
            fetched = true;
            break;
        }
    }
    delete hQuery;

    return fetched
}