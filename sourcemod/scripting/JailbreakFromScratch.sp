#define PLUGIN_NAME         "Jailbreak From Scratch"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Minimal TF2 Jailbreak plugin"

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

#include <JBFS/jbfs_vars>
#include <JBFS/jbfs_events>
#include <JBFS/jbfs_commands>
#include <JBFS/jbfs_stocks>
#include <JBFS/stocks>

#include <morecolors>

public void OnPluginStart()
{
    //register cvars
    cvarJBFS[BalanceRatio] = CreateConVar("sm_jbfs_balanceratio","0.5","Default balance ratio",FCVAR_NOTIFY,true,0.1,true,1.0);
    cvarJBFS[Version] = CreateConVar("jbfs_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);
    AutoExecConfig(true,"JBFS");

    //regular commands
    RegConsoleCmd("sm_w",Command_Warden);
    RegConsoleCmd("sm_warden",Command_Warden);
    RegConsoleCmd("sm_uw",Command_UnWarden);
    RegConsoleCmd("sm_unwarden",Command_UnWarden);
    //admin commands

    //hook gameevents for use as functions
    HookEvent("teamplay_round_start",OnPreRoundStart);
    HookEvent("arena_round_start",OnArenaRoundStart);
    HookEvent("player_disconnect",OnPlayerDisconnect);
    HookEvent("player_death",OnPlayerDeath);
    HookEvent("player_spawn",OnPlayerSpawn);

    SetConVars(true);

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);

    //import translations
    LoadTranslations("common.phrases");
    LoadTranslations("jbfs.phrases");
}

public void OnPluginEnd()
{
    SetConVars(false);
}