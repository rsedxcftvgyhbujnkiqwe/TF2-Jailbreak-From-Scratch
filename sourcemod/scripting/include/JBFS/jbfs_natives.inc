public any Native_AddGuardBan(Handle plugin, int numParams)
{
    return AddGuardBan(GetNativeCell(1));
}

public any Native_RemoveGuardBan(Handle plugin, int numParams)
{
    return RemoveGuardBan(GetNativeCell(1));
}

public any Native_IsGuardBanned(Handle plugin, int numParams)
{
    return IsGuardBanned(GetNativeCell(1));
}