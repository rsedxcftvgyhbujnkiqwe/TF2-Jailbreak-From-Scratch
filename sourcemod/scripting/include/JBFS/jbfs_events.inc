public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    AutoBalanceTeams();
}

public void OnClientSpeaking(int client)
{
    HasMic[client] = true;
}