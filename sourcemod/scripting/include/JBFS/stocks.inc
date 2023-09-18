//get number of players by team
stock int GetNumPlayers(const int team=0,const bool alive=false)
{
    int AlivePlayers = 0;
    //iterate through player indexes
    for (int i = MaxClients ; i; --i)
    {
        if (!IsClientInGame(i))
            continue;
        if (team && GetClientTeam(i) != team) 
            continue;
        if (alive && !IsPlayerAlive(i))
            continue;
        ++AlivePlayers;
    }
    return AlivePlayers;
}

//get list of player indexes by team
stock void GetPlayers(const int team=0,const bool alive=false,int[] clients,int clientsize)
{
    int clientCount;
    for (int i = MaxClients; i; --i)
    {
        if (!IsClientInGame(i))
            continue;
        if (team && GetClientTeam(i) != team) 
            continue;
        if (alive && !IsPlayerAlive(i))
            continue;
        clients[clientCount++] = i;
    }
}

//get random player by team
stock int GetRandomPlayer(const int team=0,const bool alive=false)  
{
    int playercount = GetNumPlayers(team,alive);
    int[] players = new int[playercount];
    GetPlayers(team,alive,players,playercount);
    return (playercount == 0) ? -1 : players[GetRandomInt(0, playercount - 1)];
}

//force player to change teams, and respawn
stock void ForceTeamChange(const int client, const int team,const bool spawn = true )
{
    if (TF2_GetPlayerClass(client) > TFClass_Unknown) 
    {
        SetEntProp(client, Prop_Send, "m_lifeState", 2);
        ChangeClientTeam(client, team);
        SetEntProp(client, Prop_Send, "m_lifeState", 0);
        if (spawn)
            TF2_RespawnPlayer(client);
    }
}