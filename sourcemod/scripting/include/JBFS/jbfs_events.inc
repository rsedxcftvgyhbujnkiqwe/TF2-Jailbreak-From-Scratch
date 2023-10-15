public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    LRConfig.LRActive = LRConfig.LRChosen;
    LRConfig.LRChosen = LR_None;
    JBFS.RoundActive = false;
    //warden stuff
    RemoveWarden(UW_Silent);
    LockWarden(false);
    SetArray(JBFS.FireStatus,sizeof(JBFS.FireStatus),false);
    ResetActiveLR();
    //send warden set 0
    JBGameEvent(JBEvent_SetWarden);
    //reset convars
    FindConVar("mp_friendlyfire").SetBool(false);
    //fire preround lr stuff (freedays)
    if(LRConfig.LRActive == LR_FreedaySelf || LRConfig.LRActive == LR_FreedayOthers)
    {
        for (int i;i<3;i++)
        {
            int client = LRConfig.Freedays[i]
            if(client == 0 || !IsClientInGame(client)) continue;
            LRConfig.FreedaysActive[client] = true;
            CPrintToChatAll("%t %t","PluginTag","LRFreedayActive",client);
            if(GetClientTeam(client)!=RED) continue;
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
    switch (LRConfig.LRActive)
    {
        case LR_None: CPrintToChatAll("%t %t","PluginTag","LRNoneActive");
        case LR_FreedaySelf: CPrintToChatAll("%t %t","PluginTag","LRFreedaySelfActive");
        case LR_FreedayOthers: CPrintToChatAll("%t %t","PluginTag","LRFreedayOthersActive");
        case LR_FreedayAll:
        {
            CPrintToChatAll("%t %t","PluginTag","LRFreedayAllActive");
            //open cells
            ModifyCells(true,0,true);
            //kill cells to prevent them from being closed
            CreateTimer(5.0,Timer_KillCells);
            JBFS.TimeLeft = cvarJBFS[FreedayTime].IntValue;
        }
        case LR_Warday:
        {
            LRWarday();
            JBFS.TimeLeft = cvarJBFS[WardayTime].IntValue;
            CPrintToChatAll("%t %t","PluginTag","LRWardayActive");
        }
        case LR_Custom: CPrintToChatAll("%t %t","PluginTag","LRCustomActive",LRConfig.CustomLR);
    }
    CreateTimer(5.0,Timer_AllowLRQuery);
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
    RemoveWeapons(client);
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
    int fday = FindInArray(LRConfig.Freedays,3,client)
    if (fday != -1) LRConfig.Freedays[fday] = 0;
    RemoveParticle(client);
    SetArray(g_BlockedWeapons[client],6,-1);
    g_HasBlockedWeapons[client] = false;
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
        if(LRConfig.FreedaysActive[victim]) damage = 0.0;
        //dealing damage removes your freeday
        if(LRConfig.FreedaysActive[attacker]) LRRemoveFreeday(attacker);
    }
    return action
}

public void OnClientSayCommand_Post(int client, const char[] command, const char[] argc)
{
    if (!IsChatTrigger() && LRConfig.CustomChatWait){
        if (LRConfig.CustomChatWait && LRConfig.LRWinner == client)
        {
            SetCustomLR(argc);
        }
    }
}

public void OnTakeAmmo(const char[] output, int caller, int activator, float delay)
{
    ReturnWeapons(activator);
}

public void OnEntityCreated(int entity, const char[] classname) {
    if (StrEqual(classname, "obj_teleporter") || StrEqual(classname, "obj_dispenser") || StrEqual(classname, "obj_sentrygun"))
        SDKHook(entity,SDKHook_Spawn, BuildingSpawn);
    else if (cvarJBFS[DroppedWeapon].BoolValue && StrEqual(classname, "tf_dropped_weapon"))
        SDKHook(entity, SDKHook_Spawn, KillOnSpawn);
    else if (cvarJBFS[DroppedAmmo].BoolValue && StrEqual(classname, "tf_ammo_pack"))
        SDKHook(entity, SDKHook_Spawn, KillOnSpawn);
    else if (cvarJBFS[PointServerCMD].BoolValue && StrEqual(classname, "point_servercommand"))
        SDKHook(entity, SDKHook_Spawn, KillOnSpawn);
}

stock void BuildingSpawn(int entity)
{
    int builder = GetEntPropEnt(entity, Prop_Send, "m_hBuilder")
    if (builder == -1) return;
    int team = GetEntProp(builder,Prop_Send,"m_iTeamNum");
    int building, buildteam, buildteamwarday;
    char classname[32];
    GetEdictClassname(entity,classname,sizeof(classname));
    if (StrEqual(classname, "obj_teleporter")) building = Building_Teleporter;
    else if(StrEqual(classname, "obj_dispenser")) building = Building_Dispenser;
    else if(StrEqual(classname, "obj_sentrygun")) building = Building_Sentry;
    if (team == RED)
    {
        buildteam = RedBuild;
        buildteamwarday = RedBuildWarday;
    }
    else if (team == BLU)
    {
        buildteam = BlueBuild;
        buildteamwarday = BlueBuildWarday;
    }
    if (!(cvarJBFS[buildteam].IntValue & building) && !(cvarJBFS[buildteamwarday].BoolValue && LRConfig.WardayActive))
    {
        SDKHook(builder,SDKHook_WeaponSwitchPost,BuildingSwitchHook)
    }
}

public void BuildingSwitchHook(int client)
{
    TF2Util_SetPlayerActiveWeapon(client,GetPlayerWeaponSlot(client, 3))
}

public Action KillOnSpawn(int entity) {
	AcceptEntityInput(entity, "Kill");
	return Plugin_Handled;
}