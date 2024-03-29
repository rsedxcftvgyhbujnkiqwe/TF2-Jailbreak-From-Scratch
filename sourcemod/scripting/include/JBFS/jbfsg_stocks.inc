stock void SanitizeString(char[] buffer, int maxlength,const char[] input)
{
    //assume your buffer and input are the same size!
    int pos = 0;
    for(int i;i<maxlength;i++)
    {
        if (input[i] == '%' || input[i] == '\\') continue;
        buffer[pos++] = input[i]
        if(input[i] == '\0') break;
    }
}

stock void ReduceString(char[] buffer, int maxlength, const char[] source)
{
    int bufferpos;
    for (int i;i<maxlength;i++)
    {
        char current = source[i];
        if (current >= 'A' && current <= 'Z')
            current += 32;
        if (current >= 'a' && current <= 'z')
            buffer[bufferpos++] = current;
    }
    buffer[bufferpos] = '\0';
}

stock bool ValidAlphaString(char[] string, int maxlength)
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

stock int InitLocalGang(int gang_uid)
{
    if (gang_uid < 1) return 0;
    //gangs start at 1
    //in sql anyway
    //haha
    //but also here
    int g_ind;
    int available;
    for(int i=1;i<MAXPLAYERS+1;i++)
    {
        if (g_GangData[i].id == 0 && available == 0) available = i;
        if (g_GangData[i].id==gang_uid && g_ind == 0) g_ind = i;
    }
    if (g_ind > 0) return g_ind;
    g_ind = available;
    g_GangData[g_ind].id = gang_uid;
    DB_GetGangName(gang_uid,g_GangData[g_ind].name,32);
    if(DB_GetGangTag(gang_uid,g_GangData[g_ind].tag,5))
        g_GangData[g_ind].hastag = true;
    else
        g_GangData[g_ind].hastag = false;
    return g_ind;
}

stock int GetGangIndex(int gang_uid)
{
    for (int i=1;i<MAXPLAYERS+1;i++)
    {
        if (g_GangData[i].id == gang_uid) return i;
    }
    return 0;
}

stock bool IsPlayerInGang(int client, int gang_uid)
{
    int g_ind = g_PlayerData[client].index;
    if (gang_uid > 0)
    {
        return g_GangData[g_ind].id == gang_uid;
    }
    else
    {
        return g_GangData[g_ind].id > 0;
    }
}

stock void SetPlayerGang(int client, int gang_uid, int gang_rank, int mid, bool savedatabase = true)
{
    int gang_index = InitLocalGang(gang_uid);
    g_GangData[gang_index].num_online++;
    g_PlayerData[client].index = gang_index;
    g_PlayerData[client].rank = gang_rank;
    g_PlayerData[client].mid = mid;
    if(savedatabase) DB_SetPlayerGang(client,gang_uid,gang_rank,mid);
}

stock bool SetGangName(int gang_index, char new_name[32], bool savedatabase = true)
{
    int gang_uid = g_GangData[gang_index].id;
    if(savedatabase) DB_SetGangName(gang_uid,new_name);
    g_GangData[gang_index].name = new_name;
    return true;
}

stock bool SetGangTag(int gang_index, char new_tag[5], bool savedatabase = true)
{
    int gang_uid = g_GangData[gang_index].id;
    if(savedatabase) DB_SetGangTag(gang_uid,new_tag);
    g_GangData[gang_index].tag = new_tag;
    return true;
}

stock void ResetClientGangData(int client)
{
    int gang_index = g_PlayerData[client].index
    g_PlayerData[client].index = 0;
    g_PlayerData[client].rank = GangRank_None;
    g_PlayerData[client].mid = GangRank_None;
    g_PlayerData[client].invite = GangRank_None;
    g_PlayerData[client].gc_active = false;
    //local gang db reset
    g_GangData[gang_index].num_online--;
    if (g_GangData[gang_index].num_online <= 0)
    {
        ResetGangData(gang_index)
    }
}

//this nulls the gang at the index
//make sure no players are in this gang!
stock void ResetGangData(int gang_index)
{
    g_GangData[gang_index].id = 0;
    g_GangData[gang_index].name = "\0"
    g_GangData[gang_index].tag = "\0"
    g_GangData[gang_index].hastag = false;
    g_GangData[gang_index].boss_promotion = 0;
    g_GangData[gang_index].alive_red = 0;
    g_GangData[gang_index].alive_blue = 0;
    g_GangData[gang_index].shopbought = 0;
    g_GangData[gang_index].shopactive = 0;
}

stock CPrintToChatGang(int gang_uid, const String:message[], any:...) {
    int gang_index = GetGangIndex(gang_uid);
    CCheckTrie();
    decl String:buffer[MAX_BUFFER_LENGTH], String:buffer2[MAX_BUFFER_LENGTH];
    for(int i = 1; i <= MaxClients; i++) {
        if(!IsClientInGame(i) || g_PlayerData[i].index != gang_index) {
            continue;
        }
        SetGlobalTransTarget(i);
        Format(buffer, sizeof(buffer), "\x01%s", message);
        VFormat(buffer2, sizeof(buffer2), buffer, 3);
        CReplaceColorCodes(buffer2);
        CSendMessage(i, buffer2);
    }
}

stock int GetClientByMID(int gang_uid, int mid)
{
    int gang_index = GetGangIndex(gang_uid)
    for(int i = 1; i <= MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        if (g_PlayerData[i].index == gang_index && g_PlayerData[i].mid == mid) return i;
    }
    return 0
}

stock void AddGangPoints(int gang_index, int points, char[] reason)
{
    if (gang_index > 0)
    {
        g_GangData[gang_index].points += points;
        CPrintToChatGang(g_GangData[gang_index].id,"%t %t","GangTag",reason,points)
    }
}

stock void ListGang(int client)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id
    //this will suck
    char query[512];
    SQL_FormatQuery(hDatabase,query,sizeof(query),
                "SELECT name, steamid, rank, mid "
            ... "FROM %s "
            ... "WHERE gang_uid = %d "
            ... "ORDER BY mid ASC",PlayerTable,gang_uid);

    DBResultSet hQuery = SQL_Query(hDatabase,query);
    if (hQuery == null)
    {
        char qerror[255];
        SQL_GetError(hDatabase, qerror, sizeof(qerror));
        PrintToServer("Failed to query (error: %s)", qerror);
        return;
    }

    CPrintToChat(client,"%t %t","PluginGangTag","GangMemberList",g_GangData[gang_index].name)
    CPrintToChat(client,"%t","GangMemberListFormat")

    char ID[32]; 
    char name[32];
    int rank;
    int mid;
    while (SQL_FetchRow(hQuery))
    {
        SQL_FetchString(hQuery, 0, name, sizeof(name));
        SQL_FetchString(hQuery, 1, ID, sizeof(ID));
        rank = SQL_FetchInt(hQuery, 2);
        mid = SQL_FetchInt(hQuery, 3);
        CPrintToChat(client,"{chartreuse}(%d): {%s}%s (%s)",mid,g_RankColors[rank],name,ID);
    }
    delete hQuery;
}

stock void InviteToGang(int client, int target, int gang_uid)
{
    int target_gang_index = g_PlayerData[target].index;
    if (target_gang_index > 0) {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangInviteWarnInGang")
        return;
    }
    if (g_PlayerData[target].invite == gang_uid) {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangInviteWarnInvited")
        return;
    }
    if (DB_GetGangSize(gang_uid) >= cvarJBFS[MaxGangSize].IntValue)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangMaxSize");
        return;
    }
    //very rare edge case but possible
    if (!DB_IsGangActive(gang_uid))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangInactive");
        return;
    }

    //at this point, client is able to invite, and other player is capable of joining
    g_PlayerData[target].invite = gang_uid;
    CPrintToChat(target,"%t %t","GangTag","GangInviteReceived",client,g_GangData[target_gang_index].name);
    CPrintToChatGang(gang_uid,"%t %t","GangTag","GangInviteSent",target,g_RankColors[g_PlayerData[client].rank],client);
}

stock void CloseMenus(int client)
{

    if (IsValidHandle(g_GangMenu[client]))
    {
        delete g_GangMenu[client];
    }
    if (IsValidHandle(g_ClientMenu[client]))
    {
        delete g_ClientMenu[client];
    }
    if (IsValidHandle(g_ShopMenu[client]))
    {
        delete g_ShopMenu[client];
    }
    g_GangMenu[client] = null;
    g_ClientMenu[client] = null;
    g_ShopMenu[client] = null;
}

stock void GangShopPurchase(int client, int purchase)
{
    int gang_index = g_PlayerData[client].index;
    if (gang_index == 0) return;

    int points = g_GangData[gang_index].points;

    int purchase_bit = 1 << purchase;

    if (points < g_ShopPoints[purchase])
    {
        CPrintToChat(client,"%t %t","PluginGangTag","ShopWarnPoints");
        return;
    }
    //just in case
    if (g_GangData[gang_index].shopbought & purchase_bit)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","ShopWarnPurchased");
        return;
    }

    g_GangData[gang_index].points -= g_ShopPoints[purchase];
    g_GangData[gang_index].shopbought |= purchase_bit;

    CPrintToChatGang(gang_index,"%t %t","GangTag","ShopPickGang",g_RankColors[g_PlayerData[client].rank],client,g_ShopNames[purchase],g_GangData[gang_index].points);
    switch (purchase)
    {
        case GangShop_Warp: g_GangData[gang_index].warp_target = client;
    }
}

stock void ApplyGangBuffsClient(int client, int active)
{
    if (active & ( 1 << GangShop_Banner)) TF2_AddCondition(client,TFCond_Buffed,TFCondDuration_Infinite,0);
    if (active & ( 1 << GangShop_Conch)) TF2_AddCondition(client,TFCond_RegenBuffed,TFCondDuration_Infinite,0);
    if (active & ( 1 << GangShop_Backup)) TF2_AddCondition(client,TFCond_DefenseBuffed,TFCondDuration_Infinite,0);
}

stock void ApplyGangBuff(int gang_index,int active)
{
    for (int i;i<=MaxClients;i++)
    {
        if (g_PlayerData[i].index == gang_index)
        {
            ApplyGangBuffsClient(i,active);
        }
    }
}