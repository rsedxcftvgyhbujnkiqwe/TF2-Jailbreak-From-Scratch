public void OnPreRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    //kill timer to prevent it from ending round
    if (IsValidHandle(JBFS.RoundTimer))KillTimer(JBFS.RoundTimer);
    LRConfig.LRActive = LRConfig.LRChosen;
    LRConfig.LRChosen = LR_None;
    LRConfig.LRWinner = 0; //this is done here so you can get lr winner on round end
    JBFS.RoundActive = false;  
    //surrender/rebel stuff
    SetArray(JBFS.HasSurrendered,sizeof(JBFS.HasSurrendered),false);
    SetArray(JBFS.IsRebel,sizeof(JBFS.IsRebel),false)
    for (int i;i<=MaxClients;i++)
    {
        if (IsValidHandle(JBFS.SurrenderTimers[i])) KillTimer(JBFS.SurrenderTimers[i]);
        if (IsValidHandle(JBFS.RebelTimer[i])) KillTimer(JBFS.RebelTimer[i]);
    }

    //warden stuff
    RemoveWarden(UW_Silent);
    LockWarden(false);
    SetArray(JBFS.FireStatus,sizeof(JBFS.FireStatus),false);
    ResetActiveLR();
    //unset warden mute if it's active
    if (JBFS.WardenMuteActive) ResetWardenMute();
    //send warden set -1
    JBGameEvent(JBEvent_SetWarden);
    //reset convars
    FindConVar("mp_friendlyfire").SetBool(false);
    FindConVar("tf_avoidteammates_pushaway").SetBool(false);
    FindConVar("tf_arena_round_time").SetInt(cvarJBFS[RoundTime].IntValue);
    //fire preround lr stuff (freedays)
    if(LRConfig.LRActive == LR_FreedaySelf || LRConfig.LRActive == LR_FreedayOthers)
    {
        for (int i;i<3;i++)
        {
            LRSetFreeday(LRConfig.Freedays[i])
        }
    }
}

public void OnArenaRoundStart(Event event, const char[] name, bool dontBroadcast)
{
    //this should be called to spawn the timer initially
    SetRoundTime(cvarJBFS[RoundTime].IntValue);
    JBFS.RoundActive = true;
    //initial values
    AutoBalanceTeams();
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
            CreateTimer(3.0,Timer_KillCells,_,TIMER_FLAG_NO_MAPCHANGE);
            SetRoundTime(cvarJBFS[FreedayTime].IntValue);
        }
        case LR_Warday:
        {
            LRWarday();
            SetRoundTime(cvarJBFS[WardayTime].IntValue);
            CPrintToChatAll("%t %t","PluginTag","LRWardayActive");
        }
        case LR_Custom: CPrintToChatAll("%t %t","PluginTag","LRCustomActive",LRConfig.CustomLR);
        case LR_Map:
        {
            int relay = FindEntityByName(-1, "logic_relay", g_MapLRRelays[LRConfig.MapLRChosen])
            AcceptEntityInput(relay,"Trigger");
        }
    }
    CreateTimer(3.0,Timer_ChooseWarden,_,TIMER_FLAG_NO_MAPCHANGE);
}

public void OnArenaRoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    //kill timer to prevent it from ending round
    if (IsValidHandle(JBFS.RoundTimer))KillTimer(JBFS.RoundTimer);
    LockWarden(false);
    JBFS.RoundActive = false;
    //reset lr stuff
    LRConfig.WardayActive=false;
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
    if (!IsClientInGame(client) || (GetClientTeam(client)!=RED && GetClientTeam(client)!=BLU))
        return Plugin_Continue;

    if (GetClientTeam(client)==RED)
    {
        if (LRConfig.FreedaysActive[client])
        {
            LRSetFreedayPost(client);
        }
    }
    else if (GetClientTeam(client)==BLU)
    {
        if (JBFS.GuardBanned[client])
        {
            ForceTeamChange(client,RED);
        }
    }

    InitClientAttributes(client);
    if (cvarJBFS[AirblastImmunity].BoolValue) TF2Attrib_SetByDefIndex(client, 329, 0.0);

    RemoveWeapons(client);
    JBFS.HasAmmo[client] = true;
    if (GetClientTeam(client)==RED)
    {
        if (!LRConfig.WardayActive)
            RemoveAmmo(client);
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

public void OnClientDisconnect(int client)
{
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Disconnect);
    }
    DeathEvents(client);
    JBFS.MicStatus[client] = false;
    if(JBFS.FireVotes[client]) JBFS.FireVoteCount--;
    JBFS.FireVotes[client] = false;
    JBFS.FireStatus[client] = false;
    JBFS.GuardBanned[client] = false;
}

public void DeathEvents(int client)
{
    if (LRConfig.FreedaysActive[client]) LRRemoveFreeday(client);
    int fday = FindInArray(LRConfig.Freedays,3,client)
    if (fday != -1) LRConfig.Freedays[fday] = 0;
    RemoveParticle(client);
    SetArray(g_BlockedWeapons[client],6,-1);
    g_HasBlockedWeapons[client] = false;
    if (IsValidHandle(JBFS.RebelTimer[client]))KillTimer(JBFS.RebelTimer[client]);
    if (JBFS.IsRebel[client]) ResetRebelStatus(client);
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
            if(cvarJBFS[GuardCrits].BoolValue)
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
        if(LRConfig.FreedaysActive[victim])
        {
            damage = 0.0;
            action = Plugin_Changed;
        }
        //dealing damage removes your freeday
        bool FreedayRemoved = false
        if(LRConfig.FreedaysActive[attacker] && damage > 0)
        {
            LRRemoveFreeday(attacker);
            CPrintToChatAll("%t %t","PluginTag","FreedayLost",attacker)
            FreedayRemoved = true
        }
        if (GetClientTeam(attacker)==RED && GetClientTeam(victim)==BLU)
        {
            SetRebelStatus(attacker,FreedayRemoved)
        }
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

public void AmmoTaken(int client, bool return_weapons)
{
    JBFS.HasAmmo[client] = true;
    if (return_weapons) ReturnWeapons(client);
    switch (TF2_GetPlayerClass(client))
    {
        case TFClass_Scout:
        {
            if (cvarJBFS[DoubleJump].IntValue==1)
                TF2Attrib_RemoveByDefIndex(client,49)
        }
    }
    bool FreedayRemoved = false;
    if (LRConfig.FreedaysActive[client])
    {
        LRRemoveFreeday(client);
        CPrintToChatAll("%t %t","PluginTag","FreedayLost",client)
        FreedayRemoved = true;
    }
    if (cvarJBFS[RebelAmmo].BoolValue) SetRebelStatus(client,FreedayRemoved);
}

public void OnTakeAmmo(const char[] output, int caller, int activator, float delay)
{
    AmmoTaken(activator,true)
}

public void OnTakeWeapon(int player, int weapon)
{
    int owner = GetEntPropEnt(weapon, Prop_Send, "m_hOwnerEntity");
    int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1)*4;
    int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
    if ((GetEntProp(weapon, Prop_Data, "m_iClip1") > 0) || \
        (GetEntProp(weapon, Prop_Data, "m_iClip2") > 0) || \
        (GetEntData(owner, iAmmoTable+iOffset, 4) > 0))
    {
        AmmoTaken(player,false)
    }
}

public void OnEntityCreated(int entity, const char[] classname) {
    if (StrEqual(classname, "obj_teleporter") || StrEqual(classname, "obj_dispenser") || StrEqual(classname, "obj_sentrygun"))
        SDKHook(entity,SDKHook_Spawn, BuildingSpawn);
    if (cvarJBFS[DroppedWeapon].BoolValue && StrEqual(classname, "tf_dropped_weapon"))
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
    int team = GetClientTeam(builder);
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
        RemoveEntity(entity);
    }
}

public Action KillOnSpawn(int entity) {
	AcceptEntityInput(entity, "Kill");
	return Plugin_Handled;
}

public void TF2_OnConditionAdded(int client, TFCond cond)
{
    switch (cond)
    {
        case TFCond_Cloaked:
        {
            if (IsValidEntity(JBFS.AttachedParticle[client]))
                AcceptEntityInput(JBFS.AttachedParticle[client], "Stop");
        }
        case TFCond_Disguising, TFCond_Disguised:
        {
            switch (cvarJBFS[Disguising].IntValue)
            {
                case 0:TF2_RemoveCondition(client, cond);
                case 1:if (!JBFS.HasAmmo[client]) TF2_RemoveCondition(client, cond);
                case 2:if (GetClientTeam(client) == RED) TF2_RemoveCondition(client, cond);
            }
            if (IsValidEntity(JBFS.AttachedParticle[client]))
                AcceptEntityInput(JBFS.AttachedParticle[client], "Stop");
        }
        case TFCond_Charging:
        {
            switch (cvarJBFS[DemoCharge].IntValue)
            {
                case 0:TF2_RemoveCondition(client, cond);
                case 1:if (!JBFS.HasAmmo[client]) TF2_RemoveCondition(client, cond);
                case 2:if (GetClientTeam(client) == RED) TF2_RemoveCondition(client, cond);
            }
        }

    }
}

public void TF2_OnConditionRemoved(int client, TFCond cond)
{
    switch (cond)
    {
        case TFCond_Cloaked, TFCond_Disguising, TFCond_Disguised:
        {
            if (IsValidEntity(JBFS.AttachedParticle[client]))
                AcceptEntityInput(JBFS.AttachedParticle[client], "Start");
        }
    }
}