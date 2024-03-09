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
	if (!IsValidEntity(entity))
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

stock int FindEntityByName(int e, const char[] classname, const char[] targetname)
{
    char buffer[64];
    while ((e = FindEntityByClassname(e, classname)) != -1)
    {
        GetEntPropString(e, Prop_Data, "m_iName", buffer, sizeof(buffer));
        if (StrEqual(buffer, targetname))
        {
            return e;
        }
    }
    return -1;
}

stock void EntFire(const char[] targetname,const char[] input)
{
    char buffer[128];
    int e;
    while ((e = FindEntityByClassname(e, MapConfig.cellclass)) != -1)
    {
        GetEntPropString(e, Prop_Data, "m_iName", buffer, sizeof(buffer));
        if (StrEqual(buffer, targetname))
        {
            AcceptEntityInput(e, input);
        }
    }
}

stock bool TraceRayFilterPlayers(int entity, int mask, any data)
{
    return !(0 < entity <= MaxClients);
}

stock void HexToRGB(const char[] hex, int rgb[3]) {
    int hexInt = StringToInt(hex, 16);
    rgb[0] = ((hexInt >> 16) & 0xFF);
    rgb[1] = ((hexInt >>  8) & 0xFF);
    rgb[2] = ((hexInt      ) & 0xFF);
}

stock void AttachParticle(int client, char[] particle_name)
{
    int particle = CreateEntityByName("info_particle_system");
    float clientpos[3];
    GetEntPropVector(client,Prop_Send,"m_vecOrigin",clientpos);
    TeleportEntity(particle,clientpos,NULL_VECTOR,NULL_VECTOR);

    DispatchKeyValue(particle,"effect_name",particle_name);
    DispatchSpawn(particle);

    SetVariantString("!activator");
    AcceptEntityInput(particle, "SetParent", client);

    SetEntPropEnt(particle,Prop_Send,"m_hOwnerEntity",client);
    ActivateEntity(particle);
    AcceptEntityInput(particle,"Start");




    int curparticle = JBFS.AttachedParticle[client];
    if(curparticle > 0)
    {
        AcceptEntityInput(curparticle,"Kill");
    }
    JBFS.AttachedParticle[client] = particle;
}

stock void RemoveParticle(int client)
{
    int particle = JBFS.AttachedParticle[client];
    if (particle > 0) {
        AcceptEntityInput(particle,"Kill");
        JBFS.AttachedParticle[client]=0;
    }
}

stock int FindInArray(any[] array, int maxlength, any value)
{
    for(int i;i<maxlength;i++)
    {
        if (array[i]==value) return i;
    }
    return -1;
}

stock void SetArray(any[] array, int maxlength, any value)
{
    for(int i; i<maxlength;i++)
    {
        array[i] = value;
    }
}

stock int TF2_CreateEquipWeapon(int client, char weaponname[32],int weaponindex)
{
    int weapon = CreateEntityByName(weaponname);
    if(weapon == -1) return -1;
    SetEntProp(weapon,Prop_Send,"m_iItemDefinitionIndex",weaponindex);
    SetEntProp(weapon,Prop_Send,"m_bInitialized", 1);
    SetEntProp(weapon,Prop_Send,"m_bValidatedAttachedEntity", 1);
    SetEntPropEnt(weapon,Prop_Send,"m_hOwnerEntity",client);
    SetEntPropEnt(weapon,Prop_Send,"m_hOwner",client);
    SetEntPropEnt(weapon,Prop_Send,"m_hEffectEntity",client);
    SetEntProp(weapon,Prop_Send,"m_iTeamNum",GetClientTeam(client));
    DispatchSpawn(weapon);
    EquipPlayerWeapon(client,weapon);
    return weapon;
}

stock bool TargetInRange(int client, int target,float distance)
{
    float cpos[3];
    float tpos[3];
    GetClientAbsOrigin(client,cpos);
    GetClientAbsOrigin(target,tpos);
    float dist = GetVectorDistance(cpos,tpos);
    PrintToServer("%f >= %f",dist,distance)
    return dist >= distance;
}

stock bool IsClient(int client)
{
    return 0 < client <= MaxClients
}