public any Native_AddGuardBan(Handle plugin, int numParams)
{
    AddGuardBan(GetNativeCell(1));
}

public any Native_RemoveGuardBan(Handle plugin, int numParams)
{
    RemoveGuardBan(GetNativeCell(1));
}