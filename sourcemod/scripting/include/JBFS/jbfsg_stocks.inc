

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

stock bool IsPlayerInGang(int client, int gang_uid)
{
    if (gang_uid > 0)
    {
        return g_GangData[client].id == gang_uid;
    }
    else
    {
        return g_GangData[client].id > 0;
    }
}

stock void SetPlayerGang(int client, int gang_uid, int gang_rank, int mid, bool savedatabase = true)
{
    g_GangData[client].id = gang_uid;
    g_GangData[client].rank = gang_rank;
    g_GangData[client].mid = mid;

    DB_GetGangName(gang_uid,g_GangData[client].name,32);
    if(DB_GetGangTag(gang_uid,g_GangData[client].tag,5))
        g_GangData[client].hastag = true;
    else
        g_GangData[client].hastag = false;

    if(savedatabase) DB_SetPlayerGang(client,gang_uid,gang_rank,mid);
}

stock bool SetGangName(int gang_uid, char new_name[32], bool savedatabase = true)
{
    char current_name[32];
    DB_GetGangName(gang_uid,current_name,sizeof(current_name));

    if(savedatabase && DB_GangExists(gang_uid)) DB_SetGangName(gang_uid,new_name);
    for(int i=1;i<=MaxClients;i++)
    {
        if (g_GangData[i].id == gang_uid)
            g_GangData[i].name = new_name;
    }
    return true;
}

stock bool SetGangTag(int gang_uid, char new_tag[5], bool savedatabase = true)
{
    char current_tag[5];
    DB_GetGangTag(gang_uid,current_tag,sizeof(current_tag))

    if(savedatabase && DB_GangExists(gang_uid)) DB_SetGangTag(gang_uid,new_tag);
    for(int i=1; i<MaxClients;i++)
    {
        if (g_GangData[i].id == gang_uid)
            g_GangData[i].tag = new_tag;
    }
    return true;
}

stock void ResetClientGangData(int client)
{
    g_GangData[client].id = 0;
    g_GangData[client].rank = GangRank_None;
    g_GangData[client].name = "\0"
    g_GangData[client].tag = "\0"
    g_GangData[client].hastag = false;
}

stock CPrintToChatGang(int gang_uid, const String:message[], any:...) {
    CCheckTrie();
    decl String:buffer[MAX_BUFFER_LENGTH], String:buffer2[MAX_BUFFER_LENGTH];
    for(int i = 1; i <= MaxClients; i++) {
        if(!IsClientInGame(i) || g_GangData[i].id != gang_uid) {
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
    for(int i = 1; i <= MaxClients; i++) {
        if(!IsClientInGame(i)) {
            continue;
        }
        if (g_GangData[i].id == gang_uid && g_GangData[i].mid == mid) return i;
    }
    return 0
}