Action Command_GangHelp(int client, int args)
{
    int rank = g_PlayerData[client].rank;
    CPrintToChat(client,"%t %t","PluginGangTag","GangHelp")
    CPrintToChat(client,"%t %t","PluginGangTag","GangHelpHelp")
    CPrintToChat(client,"%t %t","PluginGangTag","GangCreateHelp")
    CPrintToChat(client,"%t %t","PluginGangTag","GangJoinHelp")
    if (rank >= GangRank_Muscle)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangLeaveHelp")
        CPrintToChat(client,"%t %t","PluginGangTag","GangChatHelp")
    }
    if (rank >= GangRank_Officer)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangInviteHelp")
    }
    if (rank <= GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangNameHelp")
        CPrintToChat(client,"%t %t","PluginGangTag","GangTagHelp")
        CPrintToChat(client,"%t %t","PluginGangTag","GangPromoteHelp")
        CPrintToChat(client,"%t %t","PluginGangTag","GangDemoteHelp")
        CPrintToChat(client,"%t %t","PluginGangTag","GangKickHelp")
    }
    return Plugin_Handled;
}

Action Command_CreateGang(int client, int args)
{
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandCreateGangUsage");
        CPrintToChat(client,"%t %t","PluginGangTag","GangNameFormat");
        CPrintToChat(client,"%t %t","PluginGangTag","GangNameExample");
        return Plugin_Handled;
    }

    if(client < 1){
        PrintToServer("[JBFS] You cannot create gangs from console.")
        return Plugin_Handled;
    }

    if (IsPlayerInGang(client,0)){
        CPrintToChat(client,"%t %t","PluginGangTag","CommandCreateGangWarnInGang")
        return Plugin_Handled;
    }

    char gangname[32], s[32];
    for (int i = 1; i <= args; i++)
    {
        GetCmdArg(i, s, sizeof(s));
        if (StrEqual(gangname,"\0")) Format(gangname,sizeof(gangname),"%s%s",gangname,s)
        else Format(gangname, sizeof(gangname), "%s %s", gangname, s);
    }

    char sanitized_gangname[32];
    SanitizeString(sanitized_gangname,sizeof(gangname),gangname);

    if (DB_GangNameExists(sanitized_gangname))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangNameExists");
        CPrintToChat(client,"%t %t","PluginGangTag","GangNameFormat");
        CPrintToChat(client,"%t %t","PluginGangTag","GangNameExample");
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
    int gang_uid = DB_CreateGang(sanitized_gangname);
    if (gang_uid == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandCreateGangError")
        return Plugin_Handled;
    }

    //this handles all the player setting - no need to be redundant
    SetPlayerGang(client,gang_uid,GangRank_Boss,1,true);

    if(cvarJBFS[AnnounceGangCreate].BoolValue)
    {
        CPrintToChatAll("%t %t","PluginGangTag","GangCreated",client,sanitized_gangname);
    }
    else
    {
        CPrintToChat(client,"%t %t","GangTag","GangCreatedSilent",sanitized_gangname);
    }
    CPrintToChat(client,"%t %t","PluginGangTag","GangCreatedHelp");
    return Plugin_Handled;
}

Action Command_SetGangTag(int client, int args)
{
    int gang_index = g_PlayerData[client].index;
    if (g_GangData[gang_index].id == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang")
        return Plugin_Handled;
    }
    if (g_PlayerData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    if (args != 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandSetTagUsage");
        CPrintToChat(client,"%t %t","PluginGangTag","GangTagFormat");
        CPrintToChat(client,"%t %t","PluginGangTag","GangTagExample");
        return Plugin_Handled;
    }
    //player is in a gang, and is the boss

    /*  gang tag is enforced differently than gang name
        tag must be:
        - 2-4 characters long
        - only contain letters
        I will not bother enforcing tags > 4, the buffer should handle it I think and auto truncate
    */
    char tag[5];
    GetCmdArg(1, tag, sizeof(tag));
    if (!ValidAlphaString(tag,sizeof(tag)))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandSetNameUsage");
        CPrintToChat(client,"%t %t","PluginGangTag","GangTagFormat");
        CPrintToChat(client,"%t %t","PluginGangTag","GangTagExample");
        return Plugin_Handled;
    }

    int gang_uid = g_GangData[gang_index].id;

    char ctag[5];
    DB_GetGangTag(gang_uid,ctag,sizeof(ctag))
    if (StrEqual(tag,ctag))
    {
        CPrintToChat(client,"%t %t","PluginTag","GangTagSame")
        return Plugin_Handled;
    }

    if (DB_GangTagExists(tag))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangTagTaken")
        return Plugin_Handled;
    }

    if(SetGangTag(gang_uid,tag,true))
        CPrintToChatGang(gang_uid,"%t %t","GangTag","GangTagChange",tag);
    return Plugin_Handled;
}

Action Command_SetGangName(int client, int args)
{
    int gang_index = g_PlayerData[client].index;
    if (g_GangData[gang_index].id == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang")
        return Plugin_Handled;
    }
    if (g_PlayerData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandSetNameUsage");
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

    char sanitized_gangname[32];
    SanitizeString(sanitized_gangname,sizeof(gangname),gangname);

    int gang_uid = g_GangData[gang_index].id;
    char current_name[32];
    DB_GetGangName(gang_uid,current_name,sizeof(current_name))
    if (StrEqual(current_name,sanitized_gangname))
    {
        CPrintToChat(client,"%t %t","PluginTag","GangNameSame")
        return Plugin_Handled;
    }
    if (DB_GangNameExists(sanitized_gangname))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangNameTaken")
        return Plugin_Handled;
    }

    if(SetGangName(gang_uid,sanitized_gangname,true))
    {
        if(cvarJBFS[AnnounceGangName].BoolValue)
            CPrintToChatAll("%t %t","PluginGangTag","GangNameChange",current_name,sanitized_gangname);
        else
            CPrintToChatGang(gang_uid,"%t %t","GangTag","GangNameChangeSilent",sanitized_gangname);
    }
    return Plugin_Handled;
}

Action Command_GangChat(int client, int args)
{
    int gang_index = g_PlayerData[client].index;
    if (!cvarJBFS[GangChat].BoolValue)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangChatDisabled");
        return Plugin_Handled;
    }
    if(g_GangData[gang_index].id == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }

    char message[255], s[32];
    for (int i = 1; i <= args; i++)
    {
        GetCmdArg(i, s, sizeof(s));
        if (StrEqual(message,"\0")) Format(message,sizeof(message),"%s",s)
        else Format(message, sizeof(message), "%s %s", message, s);
    }
    if (StrEqual(message,"\0"))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangChatUsage");
        return Plugin_Handled;
    }

    char sanitized_message[32];
    SanitizeString(sanitized_message,sizeof(message),message);
    CPrintToChatGang(g_GangData[gang_index].id,"%t {%s}%N{default}: %s","GangTag",g_RankColors[g_PlayerData[client].rank],client,sanitized_message);
    PrintToServer("(Gang Chat) %N: %s",client,sanitized_message);
    return Plugin_Handled;
}

Action Command_GangInvite(int client,int args)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id;
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangInviteUsage");
        return Plugin_Handled;
    }
    if (gang_uid==0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }
    if (g_PlayerData[client].rank != GangRank_Boss && g_PlayerData[client].rank != GangRank_Officer)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int target = FindTarget(client, arg1,true);
    if (target == -1) return Plugin_Handled;

    int target_gang_index = g_PlayerData[target].index;
    if (g_GangData[target_gang_index].id > 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangInviteWarnInGang");
        return Plugin_Handled;
    }

    if (DB_GetGangSize(gang_uid) >= cvarJBFS[MaxGangSize].IntValue)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangMaxSize");
        return Plugin_Handled;
    }
    //very rare edge case but possible
    if (!DB_IsGangActive(gang_uid))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangInactive");
        return Plugin_Handled;
    }

    //at this point, client is able to invite, and other player is capable of joining

    g_PlayerData[target].invite = gang_uid;
    CPrintToChat(target,"%t %t","GangTag","GangInviteReceived",client,g_GangData[target_gang_index].name);
    CPrintToChatGang(gang_uid,"%t %t","GangTag","GangInviteSent",target,g_RankColors[g_PlayerData[client].rank],client);
    return Plugin_Handled;
}

Action Command_GangJoin(int client,int args)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id;
    int invite_target = g_PlayerData[client].invite;
    if (gang_uid > 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandCreateGangWarnInGang");
        return Plugin_Handled;
    }
    if (invite_target == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangJoinNoInvite");
        return Plugin_Handled;
    }
    //it could be that the gang has reached max size since the invite was sent
    if (DB_GetGangSize(invite_target) >= cvarJBFS[MaxGangSize].IntValue)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","GangMaxSize");
        return Plugin_Handled;
    }
    //player is not in a gang and has a valid invite
    CPrintToChatGang(invite_target,"%t %t","GangTag","GangInvitePlayerJoined",g_RankColors[GangRank_Muscle],client);
    SetPlayerGang(client,invite_target,GangRank_Muscle,DB_GetMaxMID(invite_target)+1,true);
    CPrintToChat(client,"%t %t","GangTag","GangInviteJoined",g_GangData[invite_target].name);
    g_PlayerData[client].invite = 0;
    return Plugin_Handled;

}

Action Command_GangLeave(int client,int args)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id
    if (gang_uid == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }
    //boss cant abandon the gang if its still full
    if (g_PlayerData[client].rank == GangRank_Boss && DB_GetGangSize(gang_uid) > 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangLeaveWarnBoss");
        return Plugin_Handled;
    }
    CPrintToChatGang(gang_uid,"%t %t","GangTag","GangPlayerLeave",client);
    DB_RemovePlayerFromGang(client,gang_uid);
    if (g_PlayerData[client].rank == GangRank_Boss)
    {
        DB_SetGangStatus(gang_uid,0);
        if (cvarJBFS[AnnounceGangCreate].BoolValue)
        {
            CPrintToChatAll("%t %t","PluginGangTag","GangDisband",client,g_GangData[gang_index].name);
        }
        ResetGangData(gang_index);
    }
    ResetClientGangData(client)
    return Plugin_Handled;
}

Action Command_GangList(int client,int args)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id
    if (gang_uid == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }
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
        return Plugin_Handled;
    }

    CPrintToChat(client,"%t %t","PluginGangTag","GangMemberList",g_GangData[gang_index].name)

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
        CPrintToChat(client,"{genuine}(%d): {%s}%s (%s)",mid,g_RankColors[rank],name,ID);
    }
    delete hQuery;
    return Plugin_Handled;
}

Action Command_GangKick(int client, int args)
{
    int gang_index = g_PlayerData[client].index;
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangKickUsage");
        return Plugin_Handled;
    }
    int gang_uid = g_GangData[gang_index].id
    if (gang_uid == 0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }
    if (g_PlayerData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int mid = StringToInt(arg1)
    if (mid < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangKickUsage");
        return Plugin_Handled;
    }
    if (g_PlayerData[client].mid == mid)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangWarnSelf");
        return Plugin_Handled;
    }

    //lot of sql bullshit to come
    if (!DB_MIDExists(gang_uid,mid))
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangKickWarnMemberID",mid);
        return Plugin_Handled;
    }
    char ID[32]; char name[32];
    int rank;
    DB_GetInfoByMID(gang_uid,mid,ID,name,rank);

    //delete them from their gang
    DB_RemovePlayerFromGangByMID(gang_uid,mid);
    int kicked_client = GetClientByMID(gang_uid,mid)
    if (kicked_client > 0)
    {
        Format(name,sizeof(name),"%N",kicked_client)
        ResetClientGangData(kicked_client)
    }
    CPrintToChatGang(gang_uid,"%t %t","GangTag","GangPlayerKicked",name,g_GangData[gang_index].name)

    return Plugin_Handled;
}

Action Command_GangPromote(int client, int args)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id;
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangPromoteUsage");
        return Plugin_Handled;
    }
    if (gang_uid==0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }
    if (g_PlayerData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int mid = StringToInt(arg1)
    if (mid < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangPromoteUsage");
        return Plugin_Handled;
    }

    char ID[32]; char name[32];
    int target_rank;
    DB_GetInfoByMID(gang_uid,mid,ID,name,target_rank);

    int target_client = GetClientByMID(gang_uid,mid);
    if (target_client == client)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangWarnSelf");
        return Plugin_Handled;
    }
    char sanitized_name[sizeof(name)];

    if (target_client > 0)
    {
        Format(name,sizeof(name),"%N",target_client);
    }
    SanitizeString(sanitized_name,sizeof(name),name);

    if (target_rank == GangRank_Muscle)
    {
        //promote to officer
        DB_SetPlayerGangRankByMID(gang_uid,mid,GangRank_Officer);
        CPrintToChatGang(gang_uid,"%t %t","GangTag","GangPromotion",g_RankColors[GangRank_Muscle],sanitized_name,g_RankColors[GangRank_Officer],"GangRankOfficer")
        if (target_client > 0) g_PlayerData[target_client].rank++;
    }
    else if (target_rank == GangRank_Officer)
    {
        /*
        more complicated
        first off, must have a safety check
        after second command run, perform action
        promote target to boss, and demote self to officer
        */
        if (g_GangData[gang_index].boss_promotion == mid)
        {
            DB_SetPlayerGangRankByMID(gang_uid,mid,GangRank_Boss);
            DB_SetPlayerGangRankByMID(gang_uid,g_PlayerData[client].mid,GangRank_Officer);
            CPrintToChatGang(gang_uid,"%t %t","GangTag","GangStepDown",g_RankColors[GangRank_Boss],client,g_RankColors[GangRank_Boss],"GangRankBoss");
            CPrintToChatGang(gang_uid,"%t %t","GangTag","GangPromotion",g_RankColors[GangRank_Officer],sanitized_name,g_RankColors[GangRank_Boss],"GangRankBoss");
            g_PlayerData[client].rank--;
            if (target_client > 0) g_PlayerData[target_client].rank++;
        }
        else
        {
            g_GangData[gang_index].boss_promotion = mid;
            CPrintToChat(client,"%t %t","PluginGangTag","CommandGangPromoteWarnBoss")
        }
    }
    return Plugin_Handled;
}

Action Command_GangDemote(int client, int args)
{
    int gang_index = g_PlayerData[client].index;
    int gang_uid = g_GangData[gang_index].id;
    if (args < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangDemoteUsage");
        return Plugin_Handled;
    }
    if (gang_uid==0)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnNotInGang");
        return Plugin_Handled;
    }
    if (g_PlayerData[client].rank != GangRank_Boss)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandWarnGangRank");
        return Plugin_Handled;
    }
    char arg1[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int mid = StringToInt(arg1)
    if (mid < 1)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangDemoteUsage");
        return Plugin_Handled;
    }

    char ID[32]; char name[32];
    int target_rank;
    DB_GetInfoByMID(gang_uid,mid,ID,name,target_rank);

    int target_client = GetClientByMID(gang_uid,mid);
    if (target_client == client)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangWarnSelf");
        return Plugin_Handled;
    }
    char sanitized_name[sizeof(name)];

    if (target_client > 0)
    {
        Format(name,sizeof(name),"%N",target_client);
    }
    SanitizeString(sanitized_name,sizeof(name),name);

    if (target_rank == GangRank_Muscle)
    {
        CPrintToChat(client,"%t %t","PluginGangTag","CommandGangDemoteWarnMuscle");
        return Plugin_Handled;
    }
    else if (target_rank == GangRank_Officer)
    {
        DB_SetPlayerGangRankByMID(gang_uid,mid,GangRank_Muscle);
        CPrintToChatGang(gang_uid,"%t %t","GangTag","GangDemotion",g_RankColors[GangRank_Officer],sanitized_name,g_RankColors[GangRank_Muscle],"GangRankMuscle");
        if (target_client > 0) g_PlayerData[target_client].rank--;
    }
    return Plugin_Handled;
}