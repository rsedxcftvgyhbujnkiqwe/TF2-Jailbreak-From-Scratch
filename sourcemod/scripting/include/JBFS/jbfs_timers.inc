public Action Timer_ChooseWarden(Handle timer)
{
    ChooseWarden();
    return Plugin_Stop;
}

public Action Timer_LockWarden(Handle timer)
{
    LockWarden(true);
    return Plugin_Stop;
}

public Action RoundTimer(Handle timer)
{
    int TimeLeft = JBFS.TimeLeft--;
    switch (TimeLeft)
    {
        case 1,2,3,4,5,10,30,60:
        {
            char sound[PLATFORM_MAX_PATH];
            FormatEx(sound,PLATFORM_MAX_PATH,"vo/announcer_ends_%dsec.mp3",TimeLeft);
            EmitSoundToAll(sound);
        }
        case 0:
        {
            ForceTeamWin(BLU)
            return Plugin_Stop;
        }
    }
    int Minutes = TimeLeft/60;
    int Seconds = TimeLeft%60;
    if (Seconds<10) HudTextAll("%d:0%d",Minutes,Seconds);
    else HudTextAll("%d:%d",Minutes,Seconds);

    return Plugin_Continue;
}

public Action Timer_AllowMarkerPlacement(Handle timer)
{
    JBFS.MarkerPlaced = false;
    return Plugin_Stop;
}

public Action Timer_AllowLRQuery(Handle timer)
{
    LRConfig.LRPrinted = false;
    return Plugin_Stop;
}

public Action Timer_KillCells(Handle timer)
{
    if (!MapConfig.hascells) return Plugin_Stop;
    int cells = FindEntityByName(-1,"*",MapConfig.cells);
    AcceptEntityInput(cells,"Kill")
    return Plugin_Stop;
}

public Action Timer_AllowSurrender(Handle timer, int client)
{
    JBFS.HasSurrendered[client] = false;
    return Plugin_Stop;
}

public Action Timer_AllowSearch(Handle timer,int client)
{
    JBFS.BeenSearched[client] = false;
    return Plugin_Stop;
}

public Action Timer_PlayerThink(Handle timer)
{
    for(int i; i<=MaxClients; i++)
    {
        if (!IsClientInGame(i) || !IsPlayerAlive(i)) continue;

        int buttons = GetClientButtons(i);

        int target = GetClientAimTarget(i,true);

        //determine whether this player should be able to see names
        if (cvarJBFS[AimNames].IntValue > 0){
            if ((cvarJBFS[AimNames].IntValue == 1 && i == JBFS.Warden) || (cvarJBFS[AimNames].IntValue == 2 && GetClientTeam(i)==BLU))
            {
                //determine if target is valid to see the name of
                if (IsClient(target) && IsClientInGame(target) && IsPlayerAlive(target) && GetClientTeam(target) == RED && TargetInRange(i,target,cvarJBFS[AimDistance].FloatValue))
                {
                    if (JBFS.AimTarget[i] == target) 
                    {
                        JBFS.AimTime[i] += 0.1;
                    }
                    else
                    {
                        JBFS.AimTarget[i] = target;
                        JBFS.AimTime[i] = 0.0;
                    }
                }
                else
                {
                    JBFS.AimTarget[i] = 0;
                    JBFS.AimTime[i] = 0.0;
                }

                if (JBFS.AimTarget[i] > 0 && JBFS.AimTime[i] >= cvarJBFS[AimTime].FloatValue)
                {
                    SetHudTextParamsEx(-1.0,0.6,0.5,{255,0,0,255},{255,0,0,255},0,0.25,0.25);
                    ShowSyncHudText(i,AimHud,"%N",target)
                }
            }   
        }
        if (cvarJBFS[WeaponSearch].BoolValue && GetClientTeam(i) == BLU)
        {
            if (IsClient(target) && !JBFS.BeenSearched[target])
            {
                if (IsClientInGame(target) && IsPlayerAlive(target) && GetClientTeam(target) == RED && TargetInRange(i,target,128.0))
                {
                    if (JBFS.SearchTarget[i] == target && !(g_PrevButtons[i] & IN_RELOAD) && (buttons & IN_RELOAD))
                    {
                        JBFS.SearchTime[i] += 0.1;

                        SetHudTextParamsEx(-1.0,0.65,0.5,{255,0,0,255},{255,0,0,255},0,0.25,0.25);
                        ShowSyncHudText(i,SearchHud,"%t","SearchProgress",target) 
                    }
                    else
                    {
                        JBFS.SearchTime[i] = 0.0;
                        JBFS.SearchTarget[i] = target;
                    }
                }
                else
                {
                    JBFS.SearchTime[i] = 0.0;
                    JBFS.SearchTarget[i] = 0;
                }
                if (target > 0 && JBFS.SearchTime[i] >= cvarJBFS[SearchTime].FloatValue)
                {
                    JBFS.BeenSearched[target] = true;
                    CreateTimer(30.0,Timer_AllowSearch,target,TIMER_FLAG_NO_MAPCHANGE);
                    //player has been searched, perform search stuff
                    SetHudTextParamsEx(-1.0,0.65,5.0,{255,0,0,255},{255,0,0,255},0,0.25,0.25);
                    if (JBFS.HasAmmo[target])
                    {
                        ShowSyncHudText(i,SearchHud,"SearchFound",target);
                        CPrintToChatAll("%t %t","PluginTag","SearchAnnounceFound",target)
                    }
                    else
                    {
                        ShowSyncHudText(i,SearchHud,"SearchNotFound",target);
                    }
                    JBFS.SearchTime[i] = 0.0;
                    JBFS.SearchTarget[i] = 0;
                }
            }
        }
        g_PrevButtons[i] = buttons;
    }
    return Plugin_Continue;
}