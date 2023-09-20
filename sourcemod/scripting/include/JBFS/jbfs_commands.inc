//Client Commands
public Action Command_Warden(int client, int args)
{
    if (GetClientTeam(client)==BLU)
    {
        if(IsPlayerAlive(client))
        {
            if(JBFS.HasMic[client])
            {
                if (!JBFS.Warden)
                {
                    if (!JBFS.WardenLocked){
                        SetWarden(client,W_Chose);
                    }
                    else
                    {
                        CPrintToChat(client,"%t %t","PluginTag","WardenWarnLocked");
                    }
                }
                else
                {
                    CPrintToChat(client,"%t %t","PluginTag","WardenWarnTaken",JBFS.Warden);
                }
            }
            else
            {
                CPrintToChat(client,"%t %t","PluginTag","WardenWarnNoMic");
            }
        }
        else
        {
            CPrintToChat(client,"%t %t","PluginTag","WardenWarnDead");
        }
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotBlue");
    }
    return Plugin_Handled;
}

//Warden Commands
public Action Command_UnWarden(int client, int args)
{
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Retire);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_OpenCells(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (ModifyCells(true,client)){
            CPrintToChatAll("%t %t","PluginTag","CellsOpen");
        }
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

public Action Command_CloseCells(int client, int args)
{
    if (client == JBFS.Warden)
    {
        if (ModifyCells(false,client)){
            CPrintToChatAll("%t %t","PluginTag","CellsClose");
        }
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
    }
    return Plugin_Handled;
}

//Admin Commands
public Action Command_Admin_ForceWarden(int client, int args)
{
    //handle no arg first
    //if none, select random
    if (args == 0)
    {
        ChooseWarden(true);
        return Plugin_Handled;
    }
    char arg[32];
    GetCmdArg(1, arg, sizeof(arg));
    int target = FindTarget(client, arg);
    if (target == -1) return Plugin_Handled;
    //is target eligible to be warden?
    if (IsClientInGame(target)&& IsPlayerAlive(target) && GetClientTeam(target)==BLU){
        if (target != JBFS.Warden)
        {
            RemoveWarden(UW_Force);
            SetWarden(target,W_Force);
        }
        else
        {
            CPrintToChat(client,"%t %t","PluginTag","WardenWarnAdminRedundant");
        }
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnAdminInvalid");
    }
    return Plugin_Handled;
}

public Action Command_Admin_ForceUnWarden(int client, int args)
{
    if (JBFS.Warden)
    {
        RemoveWarden(UW_Force);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnAdminNone");
    }
    return Plugin_Handled;
}

public Action Command_Admin_LockWarden(int client, int args)
{
    if(JBFS.WardenLockTimer)
    {
        KillTimer(JBFS.WardenLockTimer);
        JBFS.WardenLockTimer = INVALID_HANDLE;
    }
    if(JBFS.WardenLocked)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenLock")
        return Plugin_Handled;
    }
    LockWarden();
    return Plugin_Handled;
}

public Action Command_Admin_UnlockWarden(int client, int args)
{
    if(!JBFS.WardenLocked)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenUnlock")
        return Plugin_Handled;
    }
    UnlockWarden();
    return Plugin_Handled;

}

public Action Command_Admin_JailTime(int client, int args)
{
    int seconds;
    if (args < 1 && GetCmdArgIntEx(1,seconds) && seconds >= 0)
    {
        JBFS.TimeLeft = seconds;
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandUsageJTime");
    }
    return Plugin_Handled;
}

public Action Command_Admin_OpenCells(int client, int args)
{
    if (ModifyCells(true,client)){
        CPrintToChatAll("%t %t","PluginTag","CellsOpen");
    }
    return Plugin_Handled;
}

public Action Command_Admin_CloseCells(int client, int args)
{
    if (ModifyCells(false,client)){
        CPrintToChatAll("%t %t","PluginTag","CellsClose");
    }
    return Plugin_Handled;
}