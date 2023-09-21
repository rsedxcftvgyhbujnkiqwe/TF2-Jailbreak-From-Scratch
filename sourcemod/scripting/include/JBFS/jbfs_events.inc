public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    JBFS.Warden = 0;
    JBFS.WardenLocked = true;
    //send plugin load
    SetEventInt(JBFS_Event,"owner",JBEvent_PluginLoad);
    SetEventInt(JBFS_Event,"attacker",0);
    FireEvent(JBFS_Event);
    //send warden set 0
    SetEventInt(JBFS_Event,"owner",JBEvent_PlayerSetWarden);
    SetEventInt(JBFS_Event,"attacker",0);
    FireEvent(JBFS_Event);
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    AutoBalanceTeams();
    JBFS.TimeLeft = cvarJBFS[RoundTime].IntValue;
    JBFS.RoundTimer = CreateTimer(1.0,RoundTimer,_,TIMER_REPEAT);
    JBFS.WardenLockTimer = CreateTimer(3.0,Timer_ChooseWarden);
}

public void OnArenaRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    if (IsValidHandle(JBFS.RoundTimer)) KillTimer(JBFS.RoundTimer);
    if (IsValidHandle(JBFS.WardenLockTimer)) KillTimer(JBFS.WardenLockTimer);
}

public OnClientPutInServer(client)
{
    SDKHook(client,SDKHook_OnTakeDamage,OnTakeDamage);
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

public Action OnPlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (!IsClientInGame(client))
        return Plugin_Continue;
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Death);
    }
    return Plugin_Handled;
}

public void OnPlayerDisconnect(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Disconnect);
    }
    JBFS.HasMic[client] = false;
}

public void OnClientSpeaking(int client)
{
    JBFS.HasMic[client] = true;
}

public Action:OnTakeDamage(victim,&attacker,&inflictor,&Float:damage,&damagetype)
{
    Action action = Plugin_Continue;
    //valid player attacking?
    if(attacker && attacker <= MaxClients && IsClientInGame(attacker))
    {
        //only for blues
        if(GetClientTeam(attacker)==BLU)
        {
            if(cvarJBFS[GuardCrits])
            {
                //cases for crits:
                //is_warden    true    true    false   false
                //is_locked    true    false   false   true
                //return       true    true    true    false
                if(JBFS.Warden || !JBFS.WardenLocked)
                {
                    //crit conditions have been met
                    damagetype |= DMG_CRIT;
                    action = Plugin_Changed;
                }
            }
        }
    }
    return action
}