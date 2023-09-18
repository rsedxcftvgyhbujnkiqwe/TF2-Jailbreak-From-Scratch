//Client Commands
public Action Command_Warden(int client, int args)
{
    if (GetClientTeam(client)==BLU)
    {
        if(IsPlayerAlive(client))
        {
            if(HasMic[client])
            {
                if (!JBFS.Warden)
                {
                    if (!JBFS.WardenLocked){
                        SetWarden(client);
                    }
                    else
                    {
                        CPrintToChat(client,"%t %t","PluginTag","WardenWarnLocked");
                        return Plugin_Handled;
                    }
                }
                else
                {
                    CPrintToChat(client,"%t %t","PluginTag","WardenWarnTaken",JBFS.Warden);
                    return Plugin_Handled;
                }
            }
            else
            {
                CPrintToChat(client,"%t %t","PluginTag","WardenWarnNoMic");
                return Plugin_Handled;
            }
        }
        else
        {
            CPrintToChat(client,"%t %t","PluginTag","WardenWarnDead");
            return Plugin_Handled;
        }
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotBlue");
        return Plugin_Handled;
    }
}


public Action Command_UnWarden(int client, int args)
{
    if (client == JBFS.Warden)
    {
        RemoveWarden(UW_Retire);
    }
    else
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
        return Plugin_Handled;
    }
}

//Admin Commands