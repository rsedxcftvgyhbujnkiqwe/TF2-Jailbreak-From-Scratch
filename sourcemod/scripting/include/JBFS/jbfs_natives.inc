public any Native_AddGuardBan(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    if (JBFS.GuardBanned[client]) return false;
    JBFS.GuardBanned[client] = true;
    return true;
}

public any Native_RemoveGuardBan(Handle plugin, int numParams)
{
    int client = GetNativeCell(1);
    if (!JBFS.GuardBanned[client]) return false;
    JBFS.GuardBanned[client] = false;
    return true;
}

public any Native_IsGuardBanned(Handle plugin, int numParams)
{
    return JBFS.GuardBanned[GetNativeCell(1)];
}

public any Native_GetLRWinner(Handle plugin, int numParams)
{
    return LRConfig.LRWinner;
}

public any Native_IsWarday(Handle plugin, int numParams)
{
    return LRConfig.WardayActive;
}

public any Native_IsFreeday(Handle plugin, int numParamas)
{
    return LRConfig.FreedaysActive[GetNativeCell(1)];
}