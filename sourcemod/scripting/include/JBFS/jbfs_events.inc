public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    JBFS.RoundActive = false;
    //warden stuff
    RemoveWarden(UW_Silent);
    LockWarden(false);
    SetBoolArray(JBFS.FireStatus,MAXPLAYERS+1,false);
    SetBoolArray(LRConfig.Freedays,MAXPLAYERS+1,false)
    //send warden set 0
    JBGameEvent(JBEvent_SetWarden);
    //reset convars
    FindConVar("mp_friendlyfire").SetBool(false);
    //fire preround lr stuff (freedays)
    if(LRConfig.LRType & (LR_FreedaySelf|LR_FreedayOthers))
    {
        for (int i;i<3;i++)
        {
            int client = LRConfig.Freedays[i]
            if(!IsClientInGame(i) || GetClientTeam(i)!=RED) continue;
            LRConfig.FreedaysActive[client] = true;
            TF2_RespawnPlayer(client);
        }
    }
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    JBFS.RoundActive = true;
    //initial values
    AutoBalanceTeams();
    JBFS.TimeLeft = cvarJBFS[RoundTime].IntValue;
    //modify for LR
    if(LRConfig.LRType == LR_Warday)
    {
        LRWarday();
        JBFS.TimeLeft = cvarJBFS[WardayTime].IntValue;
    }
    JBFS.RoundTimer = CreateTimer(1.0,RoundTimer,_,TIMER_REPEAT);
    CreateTimer(3.0,Timer_ChooseWarden);
}

public void OnArenaRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    if (IsValidHandle(JBFS.RoundTimer)) KillTimer(JBFS.RoundTimer);
    LockWarden(false);
    //reset lr stuff
    LRConfig.WardayActive=false;
    LRConfig.LRWinner = 0;
    ResetActiveLR();
    for(int i=1;i<=MaxClients;i++){
        if(!IsClientInGame(i)) continue;
        RemoveParticle(i)
    }
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
        if (!LRConfig.WardayActive)
            RemoveAmmo(client);
        if (LRConfig.FreedaysActive[client])
        {
            LRSetFreeday(client);
        }
    }
    return Plugin_Handled;
}

public Action OnPlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Death);
    }
    if (!IsClientInGame(client))
        return Plugin_Continue;
    DeathEvents(client);
    return Plugin_Handled;
}

public void OnPlayerDisconnect(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Disconnect);
    }
    DeathEvents(client);
    JBFS.MicStatus[client] = false;
    if(JBFS.FireVotes[client]) JBFS.FireVoteCount--;
    JBFS.FireVotes[client] = false;
    JBFS.FireStatus[client] = false;
}

public void DeathEvents(client)
{
    if (LRConfig.FreedaysActive[client]) LRRemoveFreeday(client);
    RemoveParticle();
}

public void OnPlayerTeamChange(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_TeamChange);
    }
}

public void OnClientSpeaking(int client)
{
    JBFS.MicStatus[client] = true;
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
        //freedays take no player-player damage
        if(LRConfig.FreedaysActive[victim]) damage = 0;
        //dealing damage removes your freeday
        if(LRConfig.FreedaysActive[attacker]) LRRemoveFreeday(attacker);
    }
    return action
}