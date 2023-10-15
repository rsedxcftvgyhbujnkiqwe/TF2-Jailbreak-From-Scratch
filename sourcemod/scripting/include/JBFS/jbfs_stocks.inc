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
    JBGameEvent(JBEvent_SetWarden);
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
    AttachParticle(client,"eyeboss_vortex_blue")
    CreateWardenMenu(client,0);
}

stock void RemoveWarden(int reason)
{
    int currentwarden = JBFS.Warden;
    JBGameEvent(JBEvent_RemoveWarden);
    JBFS.Warden = 0;
    //close warden menu if it's open
    if (g_WMenu[currentwarden]!= null) delete g_WMenu[currentwarden];
    g_WMenu[currentwarden] = null;
    //reset fire votes
    SetArray(JBFS.FireStatus,sizeof(JBFS.FireStatus),false);
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
        case UW_TeamChange:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWTeamChange",currentwarden);
            UnlockWarden(true);
        }
        case UW_Silent:
        {
            UnlockWarden(false);
        }
    }
    RemoveParticle(currentwarden);
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
            if(!adminforce)
            {
                CPrintToChatAll("%t %t","PluginTag","WardenChooseNone")
                UnlockWarden(true)
            }
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
    int mwep = GetPlayerWeaponSlot(client, TFWeaponSlot_Melee);
    SetEntPropEnt(client, Prop_Send, "m_hActiveWeapon",mwep);

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
    //remove metal
    SetEntProp(client,Prop_Send,"m_iAmmo",3);
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
    if (cells == -1 && LRConfig.LRActive)
    {
        if(client) CPrintToChat(client,"%t %t","PluginTag","CellsWarnLR");
        return false;
    }
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
        if (admin) CPrintToChatAll("%t %t","PluginTag","CCNotifyOffAdmin");
        else CPrintToChatAll("%t %t","PluginTag","CCNotifyOff");
    }
    else
    {
        FindConVar("tf_avoidteammates_pushaway").SetBool(true);
        if (admin) CPrintToChatAll("%t %t","PluginTag","CCNotifyOnAdmin");
        else CPrintToChatAll("%t %t","PluginTag","CCNotifyOn");
    }
}

stock void CreateMarker(int client)
{
    float time = cvarJBFS[MarkerTime].FloatValue;
    if (time == 0.0)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandMarkerDisabled")
        return;
    }
    float eyepos[3];
    float eyeangles[3];

    GetClientEyePosition(client, eyepos);
    GetClientEyeAngles(client,eyeangles);

    Handle trace = TR_TraceRayFilterEx(eyepos,eyeangles,MASK_SHOT,RayType_Infinite,TraceRayFilterPlayers)
    if (TR_DidHit(trace))
    {
        float endpoint[3];
        TR_GetEndPosition(endpoint,trace);
        float radius = cvarJBFS[MarkerRadius].FloatValue;
        TE_SetupBeamRingPoint(endpoint,radius,radius + 0.1,JBMarker.BeamSprite,JBMarker.HaloSprite,0,10,time,2.0,0.0,JBMarker.MarkerColorRGBA,10,0);
        TE_SendToAll();
    }
    JBFS.MarkerPlaced = true;
    CreateTimer(1.0,Timer_AllowMarkerPlacement);
}

public bool ClientEligibleForLR(int client)
{
    if (GetClientTeam(client)==RED&&IsPlayerAlive(client)) return true;
    return false;
}

stock bool SetLRWinner(int client, int warden, bool adminforce = false)
{
    if((!IsClientInGame(client) || !IsPlayerAlive(client) || !(GetClientTeam(client)==RED) || (warden != JBFS.Warden)) && !adminforce)
    {
        return false;
    }
    ResetLR();
    LRConfig.LRWinner = client;
    CreateLRMenu(client);
    if (!adminforce) CPrintToChatAll("%t %t","PluginTag","LRMenuWinner",client);
    else CPrintToChatAll("%t %t","PluginTag","LRMenuWinnerAdmin",client);
    return false;
}

stock void ChooseLR(int client,int selection)
{
    LRConfig.LRChosen = selection;
    switch (selection)
    {
        case LR_FreedaySelf:
        {
            LRConfig.Freedays[0] = client;
            CPrintToChatAll("%t %t","PluginTag","LRMenuPickedFreeday",client);
        }
        case LR_FreedayAll:
        {
            CPrintToChatAll("%t %t","PluginTag","LRMenuPickedFreedayAll",client);
        }
        case LR_FreedayOthers:
        {
            CPrintToChatAll("%t %t","PluginTag","LRMenuPickedFreedayOthers",client);
            CreateClientSelectMenu(client,ClientEligibleForFreeday,LRChooseFreeday);
        }
        case LR_Warday:
        {
            CPrintToChatAll("%t %t","PluginTag","LRMenuPickedWarday",client);
        }
        case LR_Custom:
        {
            LRConfig.CustomChatWait = true;
            CPrintToChatAll("%t %t","PluginTag","LRMenuPickedCustom",client);
        }
    }
}

stock void ResetLR()
{
    LRConfig.LRActive = LR_None;
    LRConfig.Freedays[0] = 0;
    LRConfig.Freedays[1] = 0;
    LRConfig.Freedays[2] = 0;
    LRConfig.LRWinner = 0;
    LRConfig.CustomChatWait = false;
    if (LRConfig.menu != null)
    {
        CancelMenu(LRConfig.menu)
        LRConfig.menu = null;
    }
}

stock void ResetActiveLR()
{
    LRConfig.WardayActive = false;
    SetArray(LRConfig.FreedaysActive,sizeof(LRConfig.FreedaysActive),false);
    LRConfig.CustomChatWait = false;
}

stock void LRWarday()
{
    LRConfig.WardayActive = true;
    for(int i=1;i<=MaxClients;i++)
    {
        if (!IsClientInGame(i)) continue;
        TF2_RespawnPlayer(i);
        if (GetClientTeam(i)==RED){
            if(MapConfig.hasrspawn)
            {
                TeleportEntity(i,MapConfig.rspawn,NULL_VECTOR,{0.0,0.0,0.0})
            }
        }
        else if (GetClientTeam(i)==BLU){
            if(MapConfig.hasbspawn)
            {
                TeleportEntity(i,MapConfig.bspawn,NULL_VECTOR,{0.0,0.0,0.0})
            }
        }
    }
}

stock void LRSetFreeday(int client)
{
    //by being in the freeday array, a client has invuln, so no setting it up here
    //invuln done on a per-damage basis to only apply to player attacks
    //spawn particles
    AttachParticle(client,"eyeboss_vortex_red")
    //teleport them
    if (MapConfig.hasfspawn)
    {
        TeleportEntity(client,MapConfig.fspawn,NULL_VECTOR,{0.0,0.0,0.0})
    }
}

stock void LRRemoveFreeday(int client)
{
    LRConfig.FreedaysActive[client] = false;
}

stock bool ClientEligibleForFreeday(int client)
{
    if(!IsClientInGame(client)) return false;
    int fday = FindInArray(LRConfig.Freedays,3,client);
    if (fday != -1) return false;
    return true
}

stock bool LRChooseFreeday(int client, int target)
{
    if(!IsClientInGame(client) || !IsPlayerAlive(client) || !(GetClientTeam(client)==RED) || (client != LRConfig.LRWinner))
    {
        return false;
    }
    int num = -1;
    for(int i;i<3;i++)
    {
        if (LRConfig.Freedays[i]==0)
        {
            num = i;
            break
        }
    }
    if (num == -1){
        return false;
    }
    LRConfig.Freedays[num] = target;
    CPrintToChatAll("%t %t","PluginTag","LRChooseFreedayOthers",client,target,num+1)
    if (num == 2) return false;
    return true;
}

stock void SetCustomLR(const char[] playermessage)
{
    strcopy(LRConfig.CustomLR,sizeof(LRConfig.CustomLR),playermessage);
    CPrintToChatAll("%t %t","PluginTag","LRChooseCustom",LRConfig.CustomChatWait,playermessage);
    LRConfig.CustomChatWait = false;
}

stock void RemoveWeapons(int client)
{
    int weapon;
    int index;
    bool AmmoWep;
    bool WardayWep;
    bool AllWep;
    bool HasBlocked;
    SetArray(g_BlockedWeapons[client],6,-1);
    //loop over all 6 slots
    //this is so each can be chcked and verified
    for (int i; i < 6; i++)
    {
        weapon = GetPlayerWeaponSlot(client, i);
        if (weapon != -1)
        {
            index = GetEntProp(weapon,Prop_Send,"m_iItemDefinitionIndex");
            if(FindInArray(WeaponConfig.BlockedWeapons_Ammo,sizeof(WeaponConfig.BlockedWeapons_Ammo),index)!= -1) AmmoWep = true;
            else AmmoWep = false;
            if(FindInArray(WeaponConfig.BlockedWeapons_Warday,sizeof(WeaponConfig.BlockedWeapons_Warday),index)!= -1) WardayWep = true;
            else WardayWep = false;
            if(FindInArray(WeaponConfig.BlockedWeapons_All,sizeof(WeaponConfig.BlockedWeapons_All),index)!= -1) AllWep = true;
            else AllWep = false;
            //all -> always remove
            //warday -> remove on non warday
            //ammo -> remove if red
            if (((AmmoWep && GetClientTeam(client)==RED) || (AllWep || (WardayWep && !LRConfig.WardayActive))) && (GetPlayerWeaponSlot(client, TFWeaponSlot_Melee) == weapon))
            {
                //everyone needs SOME melee weapon, so if it's banned then give a default one
                PrintToServer("%N has melee weapon %d to be removed, granting default",client,index);
                RemoveWeapon(client,i);
                HasBlocked = true;
            }
            if (AllWep || (WardayWep && !LRConfig.WardayActive))
            {
                PrintToServer("%N has weapon %d to be removed!",client,index);
                RemoveWeapon(client,i);
                HasBlocked = true;
            }
            else if (AmmoWep && GetClientTeam(client)==RED)
            {
                //DANGER! If you put a melee weapon here, prisoner will A-pose!
                PrintToServer("%N has weapon %d to be temp removed!",client,index);
                SetEntPropEnt(client,Prop_Send,"m_hMyWeapons",-1,i);
                g_BlockedWeapons[client][i] = weapon;
                HasBlocked = true;
            }
        }
    }
    if(HasBlocked) g_HasBlockedWeapons[client] = true;
    if (cvarJBFS[RemoveItem1].BoolValue) TF2_RemoveWeaponSlot(client, TFWeaponSlot_Item1);
    if (cvarJBFS[RemoveItem2].BoolValue) TF2_RemoveWeaponSlot(client, TFWeaponSlot_Item2);
}

stock void RemoveWeapon(int client, int slot)
{
    switch (slot) 
    {
        case TFWeaponSlot_Primary,TFWeaponSlot_Secondary, TFWeaponSlot_Melee:
        {
            TF2_RemoveWeaponSlot(client, slot);
            TF2_CreateEquipWeapon(client, g_WepName[slot][TF2_GetPlayerClass(client)],g_WepIndex[slot][TF2_GetPlayerClass(client)]);
        }
        case TFWeaponSlot_Grenade, TFWeaponSlot_Building, TFWeaponSlot_PDA:
        {
            TF2_RemoveWeaponSlot(client, slot);
        }
    }
}

stock void ReturnWeapons(int client)
{
    if (g_HasBlockedWeapons[client])
    {
        int weapon;
        for(int i; i<6; i++)
        {
            weapon = g_BlockedWeapons[client][i];
            if (weapon > 0)
            {
                SetEntPropEnt(client,Prop_Send,"m_hMyWeapons",weapon,i);
            }
        }
        SetArray(g_BlockedWeapons[client],6 ,-1);
        g_HasBlockedWeapons[client] = false;
    }
}