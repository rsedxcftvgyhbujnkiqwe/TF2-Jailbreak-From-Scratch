//get number of players by team
stock int GetNumPlayers(int team=0,bool alive=false)
{
    int AlivePlayers = 0;
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

//
stock void GetPlayers(int team=0,bool alive=false,int[] clients,int clientsize)
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

stock int GetRandomPlayer(int team=0,bool alive=false)  
{
    int playercount = GetNumPlayers(team,alive);
    int[] players = new int[playercount];
    GetPlayers(team,alive,players,playercount);
    return (playercount == 0) ? -1 : players[GetRandomInt(0, playercount - 1)];
}