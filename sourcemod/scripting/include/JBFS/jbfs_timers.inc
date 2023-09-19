public Action Timer_ChooseWarden(Handle timer)
{
    ChooseWarden();
    return Plugin_Stop;
}

public Action Timer_LockWarden(handle timer)
{
    LockWarden();
    return Plugin_Stop;
}