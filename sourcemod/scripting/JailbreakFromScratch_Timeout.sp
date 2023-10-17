//This is too much work right now
//maybe in another version...

#define PLUGIN_NAME         "[JBFS] Timeout"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Minimal teamban plugin for JBFS"

#include <sourcemod>
#include <sdktools>

public Plugin myinfo =
{
	name = PLUGIN_NAME,
	author = PLUGIN_AUTHOR,
	description = PLUGIN_DESCRIPTION,
	version = PLUGIN_VERSION,
	url = "https://github.com/rsedxcftvgyhbujnkiqwe/TF2-Jailbreak-From-Scratch"
};

#include <jbfs>

enum
{
    ACMD_Timeout,
    Version
}
ConVar cvarJBFS[Version+1];

int g_GuardBanRounds[MAXPLAYERS+1];

enum
{
    Round_Pre,
    Round_Active,
    Round_Post
}
int RoundStatus = Round_Pre;

enum
{
    RED=2,
    BLU
}

public void OnPluginStart()
{
    cvarJBFS[ACMD_Timeout] = CreateConVar("sm_jbfst_timeout","2","Admin flag(s) required to timeout a player.",FCVAR_NOTIFY,true,0.0,true,2097151.0)

    RegConsoleCmd("sm_timeoutstatus",Command_TimeoutStatus,"Check current timeout");
    RegAdminCmd("sm_timeout",Command_Admin_Timeout,cvarJBFS[ACMD_Timeout].IntValue,"Timeout a player from guard for a specified number of rounds");

    LoadTranslations("common.phrases");
    LoadTranslations("jbfs/jbfs.phrases");
    LoadTranslations("jbfs/jbfs.timeout");

    HookEvent("teamplay_round_start",OnPreRoundStart);
    HookEvent("arena_round_start",OnArenaRoundStart);
    HookEvent("player_disconnect",OnPlayerDisconnect);
    HookEvent("player_death",OnPlayerDeath);
    HookEvent("player_spawn",OnPlayerSpawn);
    HookEvent("teamplay_round_win",OnArenaRoundEnd);
    HookEvent("teamplay_round_stalemate",OnArenaRoundEnd);
}

public Action Command_Admin_Timeout(int client, int args)
{
    if (args < 2)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandTimeoutUsage")
        return Plugin_Handled;
    }
    char arg1[32],arg2[32];
    GetCmdArg(1, arg1, sizeof(arg1));
    int target = FindTarget(client, arg1);
    if (target == -1) return Plugin_Handled;

    GetCmdArg(2, arg2, sizeof(arg2));
    int rounds = StringToInt(arg2);
    if (rounds < 1)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandTimeoutUsage")
        return Plugin_Handled;
    }
    //no reason for this to be higher
    rounds = rounds & 0xFF

    g_GuardBanRounds[target] = rounds;

    //pre round -> will be autobalanced
    switch(RoundStatus)
    {
        case Round_Active:
        {
            if(GetClientTeam(target) == BLU) ForcePlayerSuicide(target);
        }
    }
    CShowActivity2(client, "{day9}[JBFS]", "{unique}%N{wheat} has been timed out from guard for %d rounds!",target,rounds);
    return Plugin_Handled;
}