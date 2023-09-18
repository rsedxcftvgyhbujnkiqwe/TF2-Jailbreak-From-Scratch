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
                if(HasMic[BluePlayers[i]])
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
                RemoveFromArray(NoMicBlues,RandIndex);
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
                RemoveFromArray(MicBlues,RandIndex);
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

stock bool SetWarden(int client,int reason)
{
    Warden = client;
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

stock bool RemoveWarden(int reason)
{
    Warden = 0;
    switch (reason)
    {
        case UW_Retire:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWRetire",client);
        }
        case UW_Force:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWForce",client);
        }
        case UW_Death:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWDeath",client);
        }
        case UW_Disconnect:
        {
            CPrintToChatAll("%t %t","PluginTag","WardenUWDisconnect",client);
        }
    }
}