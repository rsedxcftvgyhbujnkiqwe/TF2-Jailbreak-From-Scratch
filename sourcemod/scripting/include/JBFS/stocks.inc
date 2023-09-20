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

stock void ForceTeamWin(const int team)
{
	int entity = FindEntityByClassname(-1, "team_control_point_master");
	if (entity <= 0)
	{
		entity = CreateEntityByName("team_control_point_master");
		DispatchSpawn(entity);
		AcceptEntityInput(entity, "Enable");
	}
	SetVariantInt(team);
	AcceptEntityInput(entity, "SetWinner");
}

stock bool RemoveFromIntArray(int[] array,int length, int index)
{
    if (index < 0 || index >= length)
        return false;
    for(int i = index; i < length - 1; i++)
    {
        array[i] = array[i + 1];
    }
    array[length - 1] = 0;
    return true;
}

stock int FindEntityByName(int startEnt, const char[] targetname)
{
    //no wildcard support!!!
    //too much work! just specify what you want
    char entname[PLATFORM_MAX_PATH];
    while((startEnt=FindEntityByClassname(startEnt, "*"))!=-1)
    {
        GetEntPropString(startEnt,Prop_Data,"m_iName",entname,sizeof(entname))
        if(StrEqual(entname,targetname)) break;
    }
    return startEnt;
}