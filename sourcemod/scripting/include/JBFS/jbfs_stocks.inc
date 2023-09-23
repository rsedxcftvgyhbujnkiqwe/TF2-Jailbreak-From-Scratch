stock bool HasMic(int client,bool ignore = false)
{
    //this bool will just force the check to succeed
    //useful if this check is disabled in the cvars 
    if(ignore) return true;
    if(JBFS.MicStatus[client])
    {
        if (sourcecommspp)
        {
#if defined _sourcecomms_included
        if (SourceComms_GetClientMuteType(client)) return false;
#endif 
        }
        return true;
    }
    return false;
}

stock void AutoBalanceTeams()
{
    float flBalanceRatio = cvarJBFS[BalanceRatio].FloatValue;
    int iNumBlue = GetNumPlayers(BLU,true);
    int iNumRed = GetNumPlayers(RED,true);
    int iTotalPlayers = iNumBlue+iNumRed
    float flRatio;
    float flDiff = 1.0;
    if(iNumRed>0)
    {
        //current ratio
        flRatio = float(iNumBlue)/float(iNumRed);
        //diff between target ratio and current
        flDiff = flRatio - flBalanceRatio;
    }
    //if diff <=0, ratio is already achieved
    //edge case of 1 red 1 blue also handled
    if (flDiff <= 0 || (iTotalPlayers==2 && iNumBlue==1))
        return;
    else
    {
        //get number of blues to create final ratio
        int iTargetBlues = iTotalPlayers - RoundToCeil((iTotalPlayers)/(1+flBalanceRatio));
        //number of blues that have to be balanced to achieve the ratio
        int iBalanceNum = iNumBlue-iTargetBlues
        //deal with rounding errors
        if(iBalanceNum==0)
            return;
        else
        {
            //temp var that will be decremented
            int balancecount = iBalanceNum;
            //get all blues
            int[] BluePlayers = new int[iNumBlue];
            GetPlayers(BLU,true,BluePlayers,iNumBlue);

            //mic balancing is done by first autobalancing those without mic
            //and after that, balancing those with mic
            int[] MicBlues= new int[iNumBlue];
            int[] NoMicBlues= new int[iNumBlue];
            int MicCount, NoMicCount;

            //get arrays of mic/nomic
            for(int i;i<iNumBlue;i++)
            {
                if(HasMic(BluePlayers[i],!cvarJBFS[MicBalance].BoolValue))
                    MicBlues[MicCount++] = BluePlayers[i];
                else
                    NoMicBlues[NoMicCount++] = BluePlayers[i];
            }

            //iterate through nomics
            int NumNoMic = NoMicCount;
            for(int i;i<NoMicCount;i++)
            {
                int RandIndex = GetRandomInt(0, NumNoMic-- -1);
                int RandPlayer = NoMicBlues[RandIndex];
                ForceTeamChange(RandPlayer,RED,true);
                PrintToServer("%N (no mic) autobalanced!",RandPlayer);
                RemoveFromIntArray(NoMicBlues,NumNoMic+1,RandIndex);
                balancecount--;
                if (!balancecount)
                    return;
            }
            //iterate through mics
            int NumMic = MicCount;
            for(int i;i<MicCount;i++)
            {
                int RandIndex = GetRandomInt(0, NumMic-- -1);
                int RandPlayer = MicBlues[RandIndex]
                ForceTeamChange(RandPlayer,RED,true);
                PrintToServer("%N (mic) autobalanced!",RandPlayer);
                RemoveFromIntArray(MicBlues,NumMic+1,RandIndex);
                balancecount--;
                if (!balancecount)
                    return;
            }
        }
    }
}

stock void SetConVars(const bool side)
{
    if(side)
    {
        FindConVar("mp_stalemate_enable").SetInt(0);
        FindConVar("tf_arena_use_queue").SetInt(0);
        FindConVar("mp_teams_unbalance_limit").SetInt(0);
        FindConVar("mp_autoteambalance").SetInt(0);
        FindConVar("tf_arena_first_blood").SetInt(0);
        FindConVar("mp_scrambleteams_auto").SetInt(0);
        FindConVar("phys_pushscale").SetInt(1000);
    }
    else
    {
        FindConVar("mp_stalemate_enable").SetInt(1);
        FindConVar("tf_arena_use_queue").SetInt(1);
        FindConVar("mp_teams_unbalance_limit").SetInt(1);
        FindConVar("mp_autoteambalance").SetInt(1);
        FindConVar("tf_arena_first_blood").SetInt(1);
        FindConVar("mp_scrambleteams_auto").SetInt(1);
    }
}

stock void SetWarden(int client,int reason)
{
    JBFS.Warden = client;
    LockWarden(false)
    //we just set warden + wardenlock, so no need for this anymore
    switch (reason)
    {
        case W_Chose:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenWChose",client);
        }
        case W_Force:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenWForce",client);
        }
        case W_Random:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenWRandom",client);
        }
    }
    Handle JBFS_Event = CreateEvent("pass_free",true);
    SetEventInt(JBFS_Event,"owner",JBEvent_PlayerSetWarden);
    SetEventInt(JBFS_Event,"attacker",GetClientUserId(client));
    FireEvent(JBFS_Event,true);
    CreateWardenMenu(client,0);
}

stock void RemoveWarden(int reason)
{
    int currentwarden = JBFS.Warden;
    JBFS.Warden = 0;
    //close warden menu if it's open
    delete g_WMenu[currentwarden];
    g_WMenu[currentwarden] = null;
    //reset fire votes
    SetBoolArray(JBFS.FireStatus,MAXPLAYERS+1,false);
    JBFS.FireVoteCount = 0;
    switch (reason)
    {
        case UW_Retire:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWRetire",currentwarden);
            UnlockWarden(false);
        }
        case UW_Force:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWForce",currentwarden);
            UnlockWarden(false);
        }
        case UW_Death:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWDeath",currentwarden);
            UnlockWarden(true);
        }
        case UW_Disconnect:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWDisconnect",currentwarden);
            UnlockWarden(true);
        }
        case UW_Fire:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWFire",currentwarden);
            UnlockWarden(true);
        }
        case UW_Silent:
        {
            UnlockWarden(false);
        }
    }
    Handle JBFS_Event = CreateEvent("pass_free",true);
    SetEventInt(JBFS_Event,"owner",JBEvent_PlayerUnsetWarden);
    if (currentwarden > 0)
        SetEventInt(JBFS_Event,"attacker",GetClientUserId(currentwarden));
    else
        SetEventInt(JBFS_Event,"attacker",0);
    FireEvent(JBFS_Event,true);
}

//choose a random warden
stock void ChooseWarden(bool adminforce = false)
{
    if (!JBFS.Warden || adminforce)
    {
        //get blues
        int iNumBlue = GetNumPlayers(BLU,true);
        int[] BluePlayers = new int[iNumBlue];
        GetPlayers(BLU,true,BluePlayers,iNumBlue);

        //do nothing if no blues available
        if (iNumBlue == 0) return;

        //get arrays of mic/nomic
        int[] MicBlues= new int[iNumBlue];
        int MicCount;
        for(int i;i<iNumBlue;i++)
        {
            if(HasMic(BluePlayers[i],!cvarJBFS[MicWarden].BoolValue)) MicBlues[MicCount++] = BluePlayers[i];
        }

        if (MicCount==0){
            //nobody with mic eligible
            CPrintToChatAll("%t %t","PluginTag","WardenChooseNone")
            UnlockWarden(true)
            return;
        }
        //set client to ignore if being admin forced
        //this prevents the current warden from being re-set to warden
        int clientignore;
        if(adminforce) clientignore = JBFS.Warden;

        //number of blues who can become warden
        int NumEligible = MicCount - 1;
        if (clientignore) NumEligible--;

        //select random player until valid one selected
        int RandPlayer;
        do RandPlayer = MicBlues[GetRandomInt(0,MicCount-1)];
        while (NumEligible > 0 && RandPlayer == clientignore);

        //if only 1 mic eligible on blue, and adminforce passed, RandPlayer will be the warden
        if (RandPlayer==JBFS.Warden) return;

        if (JBFS.Warden && adminforce) RemoveWarden(UW_Force);
        SetWarden(RandPlayer,W_Random);
    }
}

stock void LockWarden(bool message = true)
{
    JBFS.WardenLocked = true;
    if (message) CPrintToChatAll("%t %t","PluginTag","WardenLock");
    if (IsValidHandle(JBFS.WardenLockTimer)) KillTimer(JBFS.WardenLockTimer);
}

stock void UnlockWarden(bool message = false)
{
    JBFS.WardenLocked = false;
    if (message) CPrintToChatAll("%t %t","PluginTag","WardenUnlock");
    //if no one goes warden in 30s, lock it
    JBFS.WardenLockTimer = CreateTimer(30.0,Timer_LockWarden);
}

stock void RemoveAmmo(int client)
{
    int weapon;
    int length = GetEntPropArraySize(client, Prop_Send, "m_hMyWeapons");
    for (int i;i < length; i++)
    {
        weapon = GetEntPropEnt(client,Prop_Send,"m_hMyWeapons",i);
        if (weapon != -1)
        {
            if (GetEntProp(weapon, Prop_Data, "m_iClip1") != -1)
                SetEntProp(weapon, Prop_Send, "m_iClip1", 0);

            if (GetEntProp(weapon, Prop_Data, "m_iClip2") != -1)
                SetEntProp(weapon, Prop_Send, "m_iClip2", 0);
            SetWeaponAmmo(weapon,0);
        }
    }
}

stock int SetWeaponAmmo(const int weapon, const int ammo)
{
	int owner = GetEntPropEnt(weapon, Prop_Send, "m_hOwnerEntity");
	if (owner <= 0)
		return 0;
	if (IsValidEntity(weapon))
	{
		int iOffset = GetEntProp(weapon, Prop_Send, "m_iPrimaryAmmoType", 1)*4;
		int iAmmoTable = FindSendPropInfo("CTFPlayer", "m_iAmmo");
		SetEntData(owner, iAmmoTable+iOffset, ammo, 4, true);
	}
	return 0;
}

stock void HudTextAll(const char[] fmt, any ...)
{
    //SetGlobalTransTarget(client) - if translation text?
    SetHudTextParamsEx(-1.0,0.1,2.0,{255,183,27,255},{255,183,27,255},0,0.0,0.0);
    char buffer[255];
    VFormat(buffer, sizeof(buffer), fmt, 2);
    for(int i = 1;i<=MaxClients;i++)
    {
        if (IsClientInGame(i))
        {
            ShowHudText(i,cvarJBFS[TextChannel].IntValue,buffer);
        }
    }
}

stock bool ModifyCells(bool open,int client = 0,bool message = true)
{
    if (!MapConfig.hascells)
    {
        if(client) CPrintToChat(client,"%t %t","PluginTag","CellsWarnNone");
        return false;
    }
    int cells = FindEntityByName(-1,"*",MapConfig.cells);
    char class[PLATFORM_MAX_PATH];
    GetEntityClassname(cells,class,PLATFORM_MAX_PATH);
    if (StrEqual(class,"func_door")||StrEqual(class,"func_door_rotating"))
    {
        if(open) AcceptEntityInput(cells,"Open");
        else AcceptEntityInput(cells,"Close");
    }
    else if (StrEqual(class,"logic_relay"))
    {
        AcceptEntityInput(cells,"Trigger");
    }
    else if (StrEqual(class,"func_breakable"))
    {
        if(open) AcceptEntityInput(cells,"Break");
        else
        {
            if(client) CPrintToChat(client,"%t %t","PluginTag","CellsCantClose");
            return false;
        }
    }
    else
    {
        if(client) CPrintToChat(client,"%t %t","PluginTag","CellsBadClass");
        return false;
    }
    if(message)
    {
        if (open) CPrintToChatAll("%t %t","PluginTag","CellsOpen");
        else CPrintToChatAll("%t %t","PluginTag","CellsClose");
    }
    return true;
}

stock void ToggleFF(bool admin = false)
{
    if (FindConVar("mp_friendlyfire").BoolValue)
    {
        FindConVar("mp_friendlyfire").SetBool(false);
        if (admin) CPrintToChatAll("%t %t","PluginTag","FFNotifyOffAdmin");
        else CPrintToChatAll("%t %t","PluginTag","FFNotifyOff");
    }
    else
    {
        FindConVar("mp_friendlyfire").SetBool(true);
        if (admin) CPrintToChatAll("%t %t","PluginTag","FFNotifyOnAdmin");
        else CPrintToChatAll("%t %t","PluginTag","FFNotifyOn");
    }
}

stock void ToggleCC(bool admin = false)
{
    if (FindConVar("tf_avoidteammates_pushaway").BoolValue)
    {
        FindConVar("tf_avoidteammates_pushaway").SetBool(false);
        if (admin) CPrintToChatAll("%t %t","PluginTag","FFNotifyOffAdmin");
        else CPrintToChatAll("%t %t","PluginTag","FFNotifyOff");
    }
    else
    {
        FindConVar("tf_avoidteammates_pushaway").SetBool(true);
        if (admin) CPrintToChatAll("%t %t","PluginTag","FFNotifyOnAdmin");
        else CPrintToChatAll("%t %t","PluginTag","FFNotifyOn");
    }
}