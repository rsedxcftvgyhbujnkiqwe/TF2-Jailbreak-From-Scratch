#include <sourcemod>
#include <sdktools>

public Plugin myinfo =
{
	name = "My First Plugin",
	author = "blank",
	description = "My first plugin ever",
	version = "1.0",
	url = "https://castaway.tf/"
};

public void OnPluginStart()
{
    PrintToServer("Hello world!");
    RegAdminCmd("sm_myslap", Command_MySlap, ADMFLAG_SLAY);
}

public Action Command_MySlap(int client, int args)
{
    char arg1[32], arg2[32];

    int damage = 0;

    GetCmdArg(1,arg1,sizeof(arg1));

    if (args >= 2)
    {
        GetCmdArg(2, arg2, sizeof(arg2));
        damage = StringToInt(arg2);
    }

    int target = FindTarget(client,arg1);
    if (target == -1)
    {
        return Plugin_Handled;
    }

    SlapPlayer(target,damage);
    ReplyToCommand(client, "[SM] You slapped %N for %d damage!", target, damage);

    return Plugin_Handled;
}
