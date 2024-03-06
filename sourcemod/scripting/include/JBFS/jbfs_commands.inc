//Client Commands
public Action Command_Warden(int client, int args)
{
    if (GetClientTeam(client)!=BLU)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotBlue");
        return Plugin_Handled;
    }
    if(!IsPlayerAlive(client))
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnDead");
        return Plugin_Handled;
    }
    if(!HasMic(client,!cvarJBFS[MicWarden].BoolValue))
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNoMic");
        return Plugin_Handled;
    }
    if (JBFS.Warden)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnTaken",JBFS.Warden);
        return Plugin_Handled;
    }
    if (JBFS.WardenLocked)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnLocked");
        return Plugin_Handled;
    }
    if (JBFS.FireStatus[client])
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnFired");
        return Plugin_Handled;
    }
    
    SetWarden(client,W_Chose);
    return Plugin_Handled;
}

public Action Command_FriendlyFireStatus(int client, int args)
{
    if (FindConVar("mp_friendlyfire").BoolValue) CPrintToChat(client,"%t %t","PluginTag","FFStatusOn");
    else CPrintToChat(client,"%t %t","PluginTag","FFStatusOff");
    return Plugin_Handled;
}

public Action Command_FireWarden(int client, int args)
{
    if(!cvarJBFS[FireVote].BoolValue)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandFireWarnDisabled")
        return Plugin_Handled;
    }
    if (JBFS.Warden == 0)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandFireWarnNone")
        return Plugin_Handled;
    }
    if (client == JBFS.Warden)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandFireWarnSelf")
        return Plugin_Handled;
    }
    if (!IsPlayerAlive(client))
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandFireWarnDead")
        return Plugin_Handled;
    }
    if (JBFS.FireVotes[client])
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandFireWarnDone")
        return Plugin_Handled;
    }
    if (!cvarJBFS[GuardFire].BoolValue && GetClientTeam(client)==BLU)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandFireWarnGuard")
        return Plugin_Handled;
    }
    //finally, assuming all checks pass, this is a valid fire vote
    //register player's fire vote
    JBFS.FireVotes[client] = true;
    JBFS.FireVoteCount++;
    //check if this was the tipping point
    float cvarRatio = cvarJBFS[FireRatio].FloatValue;
    int validclients;
    for(int i=1;i<=MaxClients;i++)
    {
        if (!IsClientInGame(i)) continue;
        int team = GetClientTeam(i);
        if(team==RED) validclients++;
        else if (cvarJBFS[GuardFire].BoolValue && team==BLU && i!=JBFS.Warden) validclients++;
    }
    if (cvarJBFS[GuardFire].BoolValue) validclients--;
    int targetVotes = RoundToCeil(validclients*cvarRatio);
    if (targetVotes==0) targetVotes = 1;
    CPrintToChatAll("%t %t","PluginTag","CommandFireNotifyVote",client,JBFS.FireVoteCount,targetVotes);

    if (JBFS.FireVoteCount >= targetVotes)
    {
        //time to fire the warden!
        RemoveWarden(UW_Fire);
    }
    return Plugin_Handled;
}

public Action Command_CheckLastRequest(int client, int args)
{
    if (!LRConfig.LRPrinted){
        LRConfig.LRPrinted = true;
        switch (LRConfig.LRActive)
        {
            case LR_None: CPrintToChatAll("%t %t","PluginTag","LRNoneActive");
            case LR_FreedaySelf: CPrintToChatAll("%t %t","PluginTag","LRFreedaySelfActive");
            case LR_FreedayOthers: CPrintToChatAll("%t %t","PluginTag","LRFreedayOthersActive");
            case LR_FreedayAll: CPrintToChatAll("%t %t","PluginTag","LRFreedayAllActive");
            case LR_Warday: CPrintToChatAll("%t %t","PluginTag","LRWardayActive");
            case LR_Custom: CPrintToChatAll("%t %t","PluginTag","LRCustomActive",LRConfig.CustomLR);
            case LR_Map: CPrintToChatAll("%t %t","PluginTag","LRMapActive",g_MapLRNames[LRConfig.MapLRChosen]);
        }
        CreateTimer(5.0,Timer_AllowLRQuery,_,TIMER_FLAG_NO_MAPCHANGE);
    }
    else CPrintToChat(client,"%t %t","PluginTag","SlowDown")
    return Plugin_Handled;
}

public Action Command_Surrender(int client, int args)
{
    if (!cvarJBFS[AllowSurrender].BoolValue)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandDisabled")
        return Plugin_Handled;
    }
    if (GetClientTeam(client) != RED)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSurrenderWarnBlue");
        return Plugin_Handled;
    }
    if (!IsPlayerAlive(client))
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSurrenderWarnDead");
        return Plugin_Handled;
    }
    if (LRConfig.WardayActive)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSurrenderWarnWarday");
        return Plugin_Handled;
    }
    if (!JBFS.RoundActive)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnRound");
        return Plugin_Handled;
    }
    if (JBFS.HasSurrendered[client])
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandSurrenderWarnRecent");
        return Plugin_Handled;
    }

    //play has ammo, is alive, on red
    JBFS.HasAmmo[client] = false;
    InitClientAttributes(client);
    RemoveWeapons(client);
    RemoveAmmo(client);
    CPrintToChatAll("%t %t","PluginTag","PlayerSurrender",client);
    JBGameEvent(JBEvent_Surrender,client);
    JBFS.HasSurrendered[client] = true;
    JBFS.SurrenderTimers[client] = CreateTimer(30.0,Timer_AllowSurrender,client,TIMER_FLAG_NO_MAPCHANGE);
    return Plugin_Handled;
}

//Warden Commands
public Action Command_UnWarden(int client, int args)
{
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Retire);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_OpenCells(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (!cvarJBFS_CMD[WCMD_Cells].BoolValue)
        {
            CPrintToChat(client,"%t %t","PluginTag","CommandDisabled");
            return Plugin_Handled;
        }
        ModifyCells(true,client);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_CloseCells(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (!cvarJBFS_CMD[WCMD_Cells].BoolValue)
        {
            CPrintToChat(client,"%t %t","PluginTag","CommandDisabled");
            return Plugin_Handled;
        }
        ModifyCells(false,client);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_ToggleFriendlyFire(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (!cvarJBFS_CMD[WCMD_FriendlyFire].BoolValue)
        {
            CPrintToChat(client,"%t %t","PluginTag","CommandDisabled");
            return Plugin_Handled;
        }
        ToggleFF(false);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_ToggleCollisions(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (!cvarJBFS_CMD[WCMD_Collisions].BoolValue)
        {
            CPrintToChat(client,"%t %t","PluginTag","CommandDisabled");
            return Plugin_Handled;
        }
        ToggleCC(false);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_WardenMarker(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (!cvarJBFS_CMD[WCMD_Marker].BoolValue)
        {
            CPrintToChat(client,"%t %t","PluginTag","CommandDisabled");
            return Plugin_Handled;
        }
        if (JBFS.MarkerPlaced) CPrintToChat(client,"%t %t","PluginTag","CommandMarkerSlow");
        else CreateMarker(client);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

//Admin Commands
public Action Command_Admin_ForceWarden(int client, int args)
{
    //handle no arg first
    //if none, select random
    if (args == 0)
    {
        char tag[64],msg[256]; 
        Format(tag,sizeof(tag),"%t","PluginTagAdmin");
        Format(msg,sizeof(msg)," %t","WardenWForceRandom");
        CShowActivity2(client, tag, msg);
        LogAction(client,-1,"\"%L\" forced a random warden to be selected.",client);
        ChooseWarden(true);
        return Plugin_Handled;
    }
    char arg[32];
    GetCmdArg(1, arg, sizeof(arg));
    int target = FindTarget(client, arg);
    if (target == -1) return Plugin_Handled;
    //is target eligible to be warden?
    if (IsClientInGame(target)&& IsPlayerAlive(target) && GetClientTeam(target)==BLU && HasMic(target,!cvarJBFS[MicWarden].BoolValue)){
        if (target != JBFS.Warden)
        {
            RemoveWarden(UW_Force);
            SetWarden(target,W_Force,client);
        }
        else
        {
            CPrintToChat(client,"%t %t","PluginTag","WardenWarnAdminRedundant");
        }
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnAdminInvalid");
    }
    return Plugin_Handled;
}

public Action Command_Admin_ForceUnWarden(int client, int args)
{
    if (JBFS.Warden)
    {
        char tag[64],msg[256]; 
        Format(tag,sizeof(tag),"%t","PluginTagAdmin");
        Format(msg,sizeof(msg)," %t","WardenUWForce",JBFS.Warden);
        CShowActivity2(client, tag, msg);
        LogAction(client,JBFS.Warden,"\"%L\" forced \"%L\" to retire from Warden.",client,JBFS.Warden);
        RemoveWarden(UW_Force);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnAdminNone");
    }
    return Plugin_Handled;
}

public Action Command_Admin_LockWarden(int client, int args)
{
    if(JBFS.WardenLocked)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenLockWarn")
        return Plugin_Handled;
    }
    LockWarden(true);
    return Plugin_Handled;
}

public Action Command_Admin_UnlockWarden(int client, int args)
{
    if(!JBFS.WardenLocked)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenUnlockWarn")
        return Plugin_Handled;
    }
    UnlockWarden(true);
    return Plugin_Handled;

}

public Action Command_Admin_JailTime(int client, int args)
{
    int seconds;
    if (args == 1 && GetCmdArgIntEx(1,seconds) && seconds >= 0)
    {
        JBFS.TimeLeft = seconds;
        char tag[64],msg[256]; 
        Format(tag,sizeof(tag),"%t","PluginTagAdmin");
        Format(msg,sizeof(msg)," %t","AdminJailTime",seconds);
        CShowActivity2(client, tag, msg);
        LogAction(client,-1,"\"%L\" set jail time to %d.",client,seconds);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandUsageJTime");
    }
    return Plugin_Handled;
}

public Action Command_Admin_OpenCells(int client, int args)
{
    ModifyCells(true,client)
    return Plugin_Handled;
}

public Action Command_Admin_CloseCells(int client, int args)
{
    ModifyCells(false,client)
    return Plugin_Handled;
}

public Action Command_Admin_ToggleFriendlyFire(int client, int args)
{
    ToggleFF(true,client);
    return Plugin_Handled;
}

public Action Command_Admin_ToggleCollisions(int client, int args)
{
    ToggleCC(true,client);
    return Plugin_Handled;
}

public Action Command_Admin_ForceFreeday(int client, int args)
{
    char arg[32];
    GetCmdArg(1, arg, sizeof(arg));
    int target = FindTarget(client, arg);
    if (target == -1) return Plugin_Handled;
    if (GetClientTeam(target)!=RED || LRConfig.FreedaysActive[target])
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandAdminFreedayWarn",target);
        return Plugin_Handled;
    }

    char tag[64],msg[256]; 
    Format(tag,sizeof(tag),"%t","PluginTagAdmin");
    Format(msg,sizeof(msg)," %t","CommandAdminForceFreeday",target);
    CShowActivity2(client, tag, msg);
    LogAction(client,target,"\"%L\" forced freeday for \"%L\".",client,target);

    float clientpos[3];
    float clientangles[3];
    GetEntPropVector(target,Prop_Send,"m_vecOrigin",clientpos);
    GetClientEyeAngles(client,clientangles);
    LRSetFreeday(target);
    TeleportEntity(target,clientpos,clientangles,NULL_VECTOR);
    return Plugin_Handled;
}