public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    JBFS.WardenLocked = true;
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    AutoBalanceTeams();
}

public void OnPlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    int client = event.GetInt("userid");
    if (GetClientTeam(client)==RED)
    {
        RemoveAmmo(client);
    }
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