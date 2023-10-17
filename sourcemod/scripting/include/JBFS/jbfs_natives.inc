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