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