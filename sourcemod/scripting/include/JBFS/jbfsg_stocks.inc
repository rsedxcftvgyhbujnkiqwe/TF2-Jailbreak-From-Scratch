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

stock void AddGangPoints(int gang_uid, int points, char[] reason)
{
    if (gang_uid > 0)
    {
        int gang_index = GetGangIndex(gang_uid);
        g_GangData[gang_index].points += points;
        CPrintToChatGang(gang_uid,"%t %t","GangTag",reason,points)
    }
}