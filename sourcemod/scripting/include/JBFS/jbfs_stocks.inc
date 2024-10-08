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

stock void SetWarden(int client,int reason, int admin=0)
{
    JBFS.Warden = client;
    JBGameEvent(JBEvent_SetWarden,client);
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
            char tag[64],msg[256]; 
            Format(tag,sizeof(tag),"%t","PluginTagAdmin");
            Format(msg,sizeof(msg)," %t","WardenWForce",client)
            CShowActivity2(admin, tag, msg);
            LogAction(admin,client,"\"%L\" forced \"%L\" to become Warden.",admin,client);
        }
        case W_Random:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenWRandom",client);
        }
    }
    AttachParticle(client,"teleporter_blue_charged_level3")
    CreateWardenMenu(client,0);
}

stock void RemoveWarden(int reason)
{
    int currentwarden = JBFS.Warden;
    JBGameEvent(JBEvent_RemoveWarden,currentwarden);
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
            if (currentwarden > 0) CPrintToChatAll("%t %t","PluginTag","WardenUWRetire",currentwarden);
            UnlockWarden(false);
        }
        case UW_Force:
        {
            if (currentwarden > 0) CPrintToChatAll("%t %t","PluginTag","WardenUWForce",currentwarden);
            UnlockWarden(false);
        }
        case UW_Death:
        {
            if (currentwarden > 0) CPrintToChatAll("%t %t","PluginTag","WardenUWDeath",currentwarden);
            UnlockWarden(true);
        }
        case UW_Disconnect:
        {
            if (currentwarden > 0) CPrintToChatAll("%t %t","PluginTag","WardenUWDisconnect",currentwarden);
            UnlockWarden(true);
        }
        case UW_Fire:
        {
            if (currentwarden > 0) CPrintToChatAll("%t %t","PluginTag","WardenUWFire",currentwarden);
            UnlockWarden(true);
        }
        case UW_TeamChange:
        {
            if (currentwarden > 0) CPrintToChatAll("%t %t","PluginTag","WardenUWTeamChange",currentwarden);
            UnlockWarden(true);
        }
        case UW_Silent:
        {
            UnlockWarden(false);
        }
    }
    RemoveParticle(currentwarden);
    //unset warden mute if it's active
    if (JBFS.WardenMuteActive) ResetWardenMute();
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
    JBFS.WardenLockTimer = CreateTimer(30.0,Timer_LockWarden,_,TIMER_FLAG_NO_MAPCHANGE);
}

stock void RemoveAmmo(int client)
{
    int mwep = GetPlayerWeaponSlot(client, TFWeaponSlot_Melee);
    TF2Util_SetPlayerActiveWeapon(client,mwep);
    JBFS.HasAmmo[client] = false;
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
    SetEntProp(client,Prop_Send,"m_iAmmo",0,_,3);

    //cvar stuff
    switch (TF2_GetPlayerClass(client))
    {
        case TFClass_Scout:
        {
            if(cvarJBFS[DoubleJump].IntValue==1)
            {
                TF2Attrib_SetByDefIndex(client, 49, 1.0);
            }
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

stock bool ModifyCells(bool open,int client = 0,bool message = true)
{
    if (!MapConfig.hascells)
    {
        if(client) CPrintToChat(client,"%t %t","PluginTag","CellsWarnNone");
        return false;
    }
    if (LRConfig.LRActive)
    {
        if(client) CPrintToChat(client,"%t %t","PluginTag","CellsWarnLR");
        return false;
    }
    if (StrEqual(MapConfig.cellclass,"func_door")||StrEqual(MapConfig.cellclass,"func_door_rotating"))
    {
        
        if(open) EntFire(MapConfig.cells,"Open");
        else EntFire(MapConfig.cells,"Close");
    }
    else if (StrEqual(MapConfig.cellclass,"logic_relay"))
    {
        EntFire(MapConfig.cells,"Trigger");
    }
    else if (StrEqual(MapConfig.cellclass,"func_breakable"))
    {
        if(open) EntFire(MapConfig.cells,"Break");
        else
        {
            if(client) CPrintToChat(client,"%t %t","PluginTag","CellsCantClose");
            return false;
        }
    }
    else
    {
        if(client) CPrintToChat(client,"%t %t","PluginTag","CellsWarnBadClass");
        return false;
    }
    if(message)
    {
        if (open) CPrintToChatAll("%t %t","PluginTag","CellsOpen");
        else CPrintToChatAll("%t %t","PluginTag","CellsClose");
    }
    return true;
}

stock void ToggleFF(bool adminforce = false, int admin = 0)
{
    
    if (FindConVar("mp_friendlyfire").BoolValue)
    {
        FindConVar("mp_friendlyfire").SetBool(false);
        if (adminforce){
            char tag[64],msg[256]; 
            Format(tag,sizeof(tag),"%t","PluginTagAdmin");
            Format(msg,sizeof(msg)," %t","FFNotifyOffAdmin");
            CShowActivity2(admin, tag, msg);
            LogAction(admin,-1,"\"%L\" turned friendly-fire on.",admin);
        }
        else CPrintToChatAll("%t %t","PluginTag","FFNotifyOff");
    }
    else
    {
        FindConVar("mp_friendlyfire").SetBool(true);
        if (adminforce){
            char tag[64],msg[256]; 
            Format(tag,sizeof(tag),"%t","PluginTagAdmin");
            Format(msg,sizeof(msg)," %t","FFNotifyOnAdmin");
            CShowActivity2(admin, tag, msg);
            LogAction(admin,-1,"\"%L\" turned friendly-fire off.",admin);
        }
        else CPrintToChatAll("%t %t","PluginTag","FFNotifyOn");
    }
}

stock void ToggleCC(bool adminforce = false, int admin = 0)
{
    if (FindConVar("tf_avoidteammates_pushaway").BoolValue)
    {
        FindConVar("tf_avoidteammates_pushaway").SetBool(false);
        if (adminforce){
            char tag[64],msg[256]; 
            Format(tag,sizeof(tag),"%t","PluginTagAdmin");
            Format(msg,sizeof(msg)," %t","CCNotifyOffAdmin");
            CShowActivity2(admin, tag, msg); 
            LogAction(admin,-1,"\"%L\" turned collisions off.",admin);
        }
        else CPrintToChatAll("%t %t","PluginTag","CCNotifyOff");
    }
    else
    {
        FindConVar("tf_avoidteammates_pushaway").SetBool(true);
        if (adminforce){
            char tag[64],msg[256]; 
            Format(tag,sizeof(tag),"%t","PluginTagAdmin");
            Format(msg,sizeof(msg)," %t","CCNotifyOnAdmin");
            CShowActivity2(admin, tag, msg); 
            LogAction(admin,-1,"\"%L\" turned collisions on.",admin);
        }
        else CPrintToChatAll("%t %t","PluginTag","CCNotifyOn");
    }
}

stock void CreateMarker(int client)
{
    float time = cvarJBFS[MarkerTime].FloatValue;
    float eyepos[3];
    float eyeangles[3];

    GetClientEyePosition(client, eyepos);
    GetClientEyeAngles(client,eyeangles);

    Handle trace = TR_TraceRayFilterEx(eyepos,eyeangles,MASK_SHOT,RayType_Infinite,TraceRayFilterPlayers)
    if (TR_DidHit(trace))
    {
        float endpoint[3];
        TR_GetEndPosition(endpoint,trace);

        endpoint[2] += 5.0;

        delete trace;
        float radius = cvarJBFS[MarkerRadius].FloatValue;
        TE_SetupBeamRingPoint(endpoint,radius,radius + 0.1,JBMarker.BeamSprite,JBMarker.HaloSprite,0,10,time,2.0,0.0,JBMarker.MarkerColorRGBA,10,0);
        TE_SendToAll();
        EmitAmbientSound("misc/rd_finale_beep01.wav", endpoint);
        EmitAmbientSound("misc/rd_finale_beep01.wav", endpoint);
    }
    JBFS.MarkerPlaced = true;
    CreateTimer(2.5,Timer_AllowMarkerPlacement,_,TIMER_FLAG_NO_MAPCHANGE);
}

public bool ClientEligibleForLR(int client)
{
    if (GetClientTeam(client)==RED&&IsPlayerAlive(client)) return true;
    return false;
}

stock bool SetLRWinner(int client, int warden, bool adminforce = false, int admin = 0)
{
    if((!IsClientInGame(client) || !IsPlayerAlive(client) || !(GetClientTeam(client)==RED) || (warden != JBFS.Warden)) && !adminforce)
    {
        return false;
    }
    ResetLR();
    LRConfig.LRWinner = client;
    CreateLRMenu(client);
    if (!adminforce) CPrintToChatAll("%t %t","PluginTag","LRMenuWinner",client);
    else{
        char tag[64],msg[256]; 
        Format(tag,sizeof(tag),"%t","PluginTagAdmin");
        Format(msg,sizeof(msg)," %t","LRMenuWinnerAdmin",client);
        CShowActivity2(admin, tag, msg);
        LogAction(admin,client,"\"%L\" forcibly gave \"%L\" last request.",admin,client);
    }
    int timeleft = cvarJBFS[LRSetTime].IntValue;
    if(timeleft > 0) SetRoundTime(timeleft);
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

stock void ChooseMapLR(int client, char selection[32])
{
    LRConfig.LRChosen = LR_Map;
    LRConfig.MapLRChosen = StringToInt(selection)
    CPrintToChatAll("%t %t","PluginTag","LRMenuPickedMap",client,g_MapLRNames[LRConfig.MapLRChosen]);
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
    if(client == 0 || !IsClientInGame(client)) return;
    LRConfig.FreedaysActive[client] = true;
    CPrintToChatAll("%t %t","PluginTag","LRFreedayActive",client);
    if(GetClientTeam(client)!=RED) return;
    //remove the timer if someone ends up getting force freeday'd while a rebel.
    if (IsValidHandle(JBFS.RebelTimer[client]))KillTimer(JBFS.RebelTimer[client]);
    TF2_RespawnPlayer(client);
}

stock void LRSetFreedayPost(int client)
{
    //by being in the freeday array, a client has invuln, so no setting it up here
    //invuln done on a per-damage basis to only apply to player attacks
    //spawn particles
    AttachParticle(client,"teleporter_red_charged_level3")
    //teleport them
    if (MapConfig.hasfspawn)
    {
        TeleportEntity(client,MapConfig.fspawn,NULL_VECTOR,{0.0,0.0,0.0})
    }
}

stock void LRRemoveFreeday(int client)
{
    LRConfig.FreedaysActive[client] = false;
    RemoveParticle(client);
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
                RemoveWeapon(client,i);
                HasBlocked = true;
            }
            if (AllWep || (WardayWep && !LRConfig.WardayActive))
            {
                RemoveWeapon(client,i);
                HasBlocked = true;
            }
            else if (AmmoWep && GetClientTeam(client)==RED && !LRConfig.WardayActive)
            {
                //DANGER! If you put a melee weapon here, prisoner will A-pose!
                SetEntPropEnt(client,Prop_Send,"m_hMyWeapons",-1,i);
                g_BlockedWeapons[client][i] = weapon;
                HasBlocked = true;
            }
        }
    }
    if(HasBlocked) g_HasBlockedWeapons[client] = true;
    if (cvarJBFS[RemoveItem1].BoolValue)
    {
        TF2_RemoveWeaponSlot(client, TFWeaponSlot_Item1);
    }
    if (cvarJBFS[RemoveItem2].BoolValue)
    {
        TF2_RemoveWeaponSlot(client, TFWeaponSlot_Item2);
    }
}

stock void RemoveWeapon(int client, int slot)
{
    switch (slot)
    {
        case TFWeaponSlot_Primary,TFWeaponSlot_Secondary, TFWeaponSlot_Melee:
        {
            TF2_RemoveWeaponSlot(client, slot);
            //melee is always replaced regardless of cvar
            //prisoners need to have some weapon
            if (slot != TFWeaponSlot_Melee && cvarJBFS[ReplaceWeapon].BoolValue)
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

stock void InitClientAttributes(int client)
{
    switch (TF2_GetPlayerClass(client))
    {
        case TFClass_Scout:
        {
            TF2Attrib_RemoveByDefIndex(client, 49);
            switch (cvarJBFS[DoubleJump].IntValue)
            {
                case 0: TF2Attrib_SetByDefIndex(client, 49, 1.0);
                case 2:
                {
                    if (GetClientTeam(client)==RED)
                        TF2Attrib_SetByDefIndex(client, 49, 1.0);
                }
            }
        }
    }
}

stock void RemoveFreeday(int client)
{
    //abstraction that serves to remove freeday
    //used by vscript
    if ( IsClientInGame(client) && IsPlayerAlive(client) && GetClientTeam(client)==RED && LRConfig.FreedaysActive[client])
    {
        LRRemoveFreeday(client);
        CPrintToChatAll("%t %t","PluginTag","FreedayLost",client)
    }
}

stock void SetRoundTime(int time)
{
    if (IsValidHandle(JBFS.RoundTimer))KillTimer(JBFS.RoundTimer);
    JBFS.RoundTimer = CreateTimer(float(time) - 0.10,Timer_EndRound,_,TIMER_FLAG_NO_MAPCHANGE);
    int timer = FindEntityByClassname(-1,"team_round_timer");
    SetVariantInt(time);
    AcceptEntityInput(timer,"SetTime");
}

stock void SetRebelStatus(int client,bool silent=false)
{
    //the rebel system itself should be handled here
    //any action that is blanket rebelling should call this
    //in reality it's not "disabled" just the client facing part of it is
    if (IsValidHandle(JBFS.RebelTimer[client]))KillTimer(JBFS.RebelTimer[client]);
    int time = cvarJBFS[RebelTime].IntValue;
    //don't run the rebel code on warday
    //waste of an edict probably, everyone's a rebel!
    if (time > 0 && !LRConfig.WardayActive)
    {
        JBFS.RebelTimer[client] = CreateTimer(float(time),Timer_RebelStatus,client,TIMER_FLAG_NO_MAPCHANGE);
        AttachParticle(client,"medic_radiusheal_red_volume")
        //easier to handle freeday removal/notifications outside of this - pass silent if necessary
        if (!JBFS.IsRebel[client]){
            if (!silent) CPrintToChatAll("%t %t","PluginTag","PrisonerRebelled",client)
        }
    }
    //we set this after because checking if it's already set is useful earlier
    JBFS.IsRebel[client] = true
}

stock void ResetRebelStatus(int client)
{
    JBFS.IsRebel[client] = false;
    RemoveParticle(client)
    CPrintToChat(client,"%t %t","PluginTag","RebelStatusRemoved");
}

stock void IssueWardenMute()
{
    JBFS.WardenMuteActive = true;
    bool adminimmune = cvarJBFS[AdminMuteImmunity].BoolValue;
    CPrintToChatAll("%t %t","PluginTag","WardenMuteOn")
    for(int i=1;i<=MaxClients;i++)
    {
        if (adminimmune && CheckCommandAccess(i,"sm_awmute",2)) continue;
        if (i != JBFS.Warden && !JBFS.WardenMuted[i] && !BaseComm_IsClientMuted(i))
        {
            JBFS.WardenMuted[i] = true;
            BaseComm_SetClientMute(i, true);
        }
    }
}

stock void ResetWardenMute()
{
    JBFS.WardenMuteActive = false;
    CPrintToChatAll("%t %t","PluginTag","WardenMuteOff")
    for(int i=1;i<=MaxClients;i++)
    {
        //someone should only get this state if they weren't muted
        //by another plugin, so this is safe to do
        if (JBFS.WardenMuted[i])
        {
            JBFS.WardenMuted[i] = false;
            BaseComm_SetClientMute(i, false);
        }
    }
}