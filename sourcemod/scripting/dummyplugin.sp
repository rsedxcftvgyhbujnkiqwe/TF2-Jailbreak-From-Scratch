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

ConVar g_cvarMySlapDamage = null;

public void OnPluginStart()
{
    PrintToServer("Hello world!");
    RegAdminCmd("sm_myslap", Command_MySlap, ADMFLAG_SLAY);
    g_cvarMySlapDamage = CreateConVar("sm_myslap_damage","5","Default slap damage");
    AutoExecConfig(true,"jbfs/plugin_myslap");
}

public Action Command_MySlap(int client, int args)
{
    char arg1[32], arg2[32];

    int damage = g_cvarMySlapDamage.IntValue;

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

    char name[MAX_NAME_LENGTH];
 
    GetClientName(target, name, sizeof(name));
 
    ShowActivity2(client, "[SM] ", "Slapped %s for %d damage!", name, damage);
    LogAction(client, target, "\"%L\" slapped \"%L\" (damage %d)", client, target, damage);

    return Plugin_Handled;
}
