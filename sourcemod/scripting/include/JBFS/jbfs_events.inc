public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    JBFS.WardenLocked = true;
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    AutoBalanceTeams();
}

public Action OnPlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (!IsClientInGame(client))
        return Plugin_Continue;
    if (GetClientTeam(client)==RED)
    {
        RemoveAmmo(client);
    }
    return Plugin_Handled;
}

public void OnPlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = event.GetInt("userid");
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Death);
    }
}

public void OnPlayerDisconnect(Event event, const char[] name, bool dontBroadcast)
{
    int client = event.GetInt("userid");
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Disconnect);
    }
    
}

public void OnClientSpeaking(int client)
{
    JBFS.HasMic[client] = true;
}