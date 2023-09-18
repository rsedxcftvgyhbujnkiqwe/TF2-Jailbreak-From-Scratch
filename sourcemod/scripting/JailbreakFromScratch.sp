#define PLUGIN_NAME         "Jailbreak From Scratch"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Modern TF2 Jailbreak plugin"

#include <sourcemod>
#include <sdktools>
#include <tf2>
#include <tf2_stocks>

public Plugin myinfo =
{
	name = PLUGIN_NAME,
	author = PLUGIN_AUTHOR,
	description = PLUGIN_DESCRIPTION,
	version = PLUGIN_VERSION,
	url = "https://github.com/rsedxcftvgyhbujnkiqwe/TF2-Jailbreak-From-Scratch"
};

enum
{
    RED=2,
    BLU
}

enum 
{
    SlapDamage=0,
    BalanceRatio,
    Version
}
ConVar cvarJBFS[Version+1];

bool HasMic[MAXPLAYERS+1];

#include <JBFS/jbfs_funcs>
#include <JBFS/jbfs_events>
#include <JBFS/jbfs_commands>
#include <JBFS/stocks>

public void OnPluginStart()
{
    cvarJBFS[SlapDamage] = CreateConVar("sm_myslap_damage","5","Default slap damage");
    cvarJBFS[BalanceRatio] = CreateConVar("sm_jbfs_balanceratio","0.5","Default balance ratio",FCVAR_NOTIFY,true,0.1,true,1.0);
    cvarJBFS[Version] = CreateConVar("jbfs_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);

    RegAdminCmd("sm_myslap", Command_MySlap, ADMFLAG_SLAY);
    RegAdminCmd("sm_ms", Command_MySlap, ADMFLAG_SLAY);
    AutoExecConfig(true,"JBFS");

    HookEvent("arena_round_start",OnArenaRoundStart);

    SetConVars(true);

    LoadTranslations("common.phrases");
}

public void OnPluginEnd()
{
    SetConVars(false);
}