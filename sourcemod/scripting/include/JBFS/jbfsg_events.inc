public void OnClientPutInServer(int client)
{
    int gang_uid = DB_GetPlayerGang(client);
    int rank = GangRank_None;
    int mid = 0;
    if (gang_uid >= 0)
    {
        rank = DB_GetPlayerGangRank(client);
        mid = DB_GetPlayerMID(client,gang_uid);
    }
    SetPlayerGang(client,gang_uid,rank,mid,true);
}

public void OnClientDisconnect(int client)
{
    ResetClientGangData(client);
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    //iterate through gangs
    for(int i;i<=MaxClients;i++)
    {
        if (g_GangData[i].id > 0)
        {
            g_GangData[i].shopactive = g_GangData[i].shopbought;
            g_GangData[i].shopbought = 0;
            int active = g_GangData[i].shopactive;
            ApplyGangBuff(i,active);
            if (active & ( 1 << GangShop_Banner)) CPrintToChatAll("%t %t","PluginGangTag","GangBuffActive",g_GangData[i].name,g_ShopNames[GangShop_Banner]);
            if (active & ( 1 << GangShop_Conch)) CPrintToChatAll("%t %t","PluginGangTag","GangBuffActive",g_GangData[i].name,g_ShopNames[GangShop_Conch]);
            if (active & ( 1 << GangShop_Backup)) CPrintToChatAll("%t %t","PluginGangTag","GangBuffActive",g_GangData[i].name,g_ShopNames[GangShop_Backup]);
        }
    }
}

public void OnArenaRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    int gang_index;
    //calculate end-round points
    for (int i=1;i<=MaxClients;i++)
    {
        gang_index = g_PlayerData[i].index;
        if(IsClientInGame(i) && IsPlayerAlive(i) && (gang_index > 0))
        {
            switch (GetClientTeam(i))
            {
                case RED: g_GangData[gang_index].alive_red++;
                case BLU: g_GangData[gang_index].alive_blue++;
            }
        }
    }
    int points;
    for (int i=1; i<MAXPLAYERS+1;i++)
    {
        if (g_GangData[i].id > 0)
        {
            points = (g_GangData[i].alive_red - g_GangData[i].alive_blue) * 50;
            if (points > 0) AddGangPoints(i,points,"GangPointsGainedAlive");
            g_GangData[i].alive_blue = 0;
            g_GangData[i].alive_red = 0;
        }
    }
    int LR_Winner = JBFS_GetLRWinner();
    int LR_GangIndex = g_PlayerData[LR_Winner].index;
    if ((LR_Winner > 0) && (LR_GangIndex > 0))
    {
        AddGangPoints(LR_GangIndex,500,"GangPointsGainedLR");
    }
}
public Action OnPlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    int gang_index = g_PlayerData[client].index;
    int active = g_GangData[gang_index].shopactive;
    //remove buffs first
    TF2_RemoveCondition(client,TFCond_Buffed);
    TF2_RemoveCondition(client,TFCond_RegenBuffed);
    TF2_RemoveCondition(client,TFCond_DefenseBuffed);
    //then check if they should be reapplied
    ApplyGangBuffsClient(client,active);
    return Plugin_Handled;
}

public Action OnPlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    //close gang/client menus on death
    CloseMenus(client)
    return Plugin_Handled;
}

public Action CP_OnChatMessage(int& author, ArrayList recipients, char[] flagstring, char[] name, char[] message, bool & processcolors, bool & removecolors)
{
    if (0 < author <= MaxClients)
    {
        int gang_index = g_PlayerData[author].index;
        if (g_PlayerData[author].gc_active && gang_index > 0)
        {
            int gang_uid = g_GangData[gang_index].id;
            recipients.Clear();
            for(int i=1;i<=MaxClients;i++)
            {
                if(g_GangData[g_PlayerData[i].index].id == gang_uid)
                {
                    recipients.Push(i);
                }
            }
            Format(name,MAXLENGTH_NAME,"%t {%s}%s","GangTag",g_RankColors[g_PlayerData[author].rank],name)
            PrintToServer("(Gang Chat) %N: %s",author,message);
            return Plugin_Changed;
        }
        if (cvarJBFS[GangTags].BoolValue)
        {
            if (gang_index > 0)
            {
                if (!StrEqual(g_GangData[gang_index].tag,"\0"))
                {
                    char sTeamColor[32];
                    PrintToServer("%N team: %d",author,GetClientTeam(author))
                    switch(GetClientTeam(author))
                    {
                        case 1: sTeamColor = "gray";
                        case 2: sTeamColor = "red";
                        case 3: sTeamColor = "blue";
                        default: sTeamColor = "default";
                    }
                    Format(name,MAXLENGTH_NAME,"{%s}[{%s}%s{%s}] %s",sTeamColor,g_RankColors[g_PlayerData[author].rank],g_GangData[gang_index].tag,sTeamColor,name);
                    return Plugin_Changed;
                }
            }
        }
    }
    return Plugin_Continue;
}