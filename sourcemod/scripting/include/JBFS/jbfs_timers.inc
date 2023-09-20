public Action Timer_ChooseWarden(Handle timer)
{
    ChooseWarden();
    return Plugin_Stop;
}

public Action Timer_LockWarden(Handle timer)
{
    LockWarden();
    return Plugin_Stop;
}

public Action RoundTimer(Handle timer)
{
    int TimeLeft = JBFS.TimeLeft--;
    int Minutes = TimeLeft/60;
    int Seconds = TimeLeft%60;
    HudTextAll("%d:%d",Minutes,Seconds);
    return Plugin_Continue;
}