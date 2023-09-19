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
            //HasMic is a global bool array for whether every client has a mic
            for(int i;i<iNumBlue;i++)
            {
                if(JBFS.HasMic[BluePlayers[i]])
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
    JBFS.WardenLocked = true;
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
}

stock void RemoveWarden(int reason)
{
    int currentwarden = JBFS.Warden;
    JBFS.Warden = 0;
    JBFS.WardenLocked = false;
    switch (reason)
    {
        case UW_Retire:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWRetire",currentwarden);
        }
        case UW_Force:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWForce",currentwarden);
        }
        case UW_Death:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWDeath",currentwarden);
        }
        case UW_Disconnect:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWDisconnect",currentwarden);
        }
    }
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