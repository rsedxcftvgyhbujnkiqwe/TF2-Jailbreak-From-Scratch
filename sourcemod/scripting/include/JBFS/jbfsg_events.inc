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
    SetPlayerGang(client,gang_uid,rank,mid,false);
}

public void OnClientDisconnect(int client)
{
    ResetClientGangData(client);
}


public void OnArenaRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    int gang_index;
    //calculate end-round points
    for (int i;i<=MaxClients;i++)
    {
        gang_index = g_PlayerData[i].index;
        if(IsClientInGame(i) && IsPlayerAlive(i) && (g_GangData[gang_index].id > 0))
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
            points = (g_GangData[i].alive_blue - g_GangData[i].alive_red) * 50;
            if (points > 0) AddGangPoints(g_GangData[i].id,points,"GangPointsGainedAlive");
        }
    }
    int LR_Winner = JBFS_GetLRWinner();
    int LR_Gang = g_GangData[g_PlayerData[LR_Winner].index].id;
    if ((LR_Winner > 0) && (LR_Gang > 0))
    {
        AddGangPoints(LR_Gang,500,"GangPointsGainedLR");
    }
}

public Action OnPlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    //close gang/client menus on death
    CloseMenus(client)
    return Plugin_Handled;
}