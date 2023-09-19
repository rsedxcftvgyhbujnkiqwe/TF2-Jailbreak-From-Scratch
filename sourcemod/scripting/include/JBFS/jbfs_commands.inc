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

//Admin Commands