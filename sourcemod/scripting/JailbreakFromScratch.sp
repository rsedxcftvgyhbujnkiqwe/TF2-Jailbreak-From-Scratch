#define PLUGIN_NAME         "Jailbreak From Scratch"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Minimal TF2 Jailbreak plugin"

#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
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
#include <JBFS/jbfs_timers>
#include <JBFS/jbfs_cfg>
#include <JBFS/stocks>

#include <morecolors>

public void OnPluginStart()
{
    PrintToServer("Starting %s, version %s",PLUGIN_NAME,PLUGIN_VERSION);
    //register cvars
    cvarJBFS[BalanceRatio] = CreateConVar("sm_jbfs_balanceratio","0.5","Default balance ratio of blues to reds.",FCVAR_NOTIFY,true,0.1,true,1.0);
    cvarJBFS[TextChannel] = CreateConVar("sm_jbfs_textchannel","4","Default text channel for JBFS Hud text.",FCVAR_NOTIFY,true,0.0,true,5.0);
    cvarJBFS[GuardCrits] = CreateConVar("sm_jbfs_guardcrits","1","Should Guards have crits.\n0 = No Crits\n1 = Crits",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[RoundTime] = CreateConVar("sm_jbfs_roundtime","600","Time per round, in seconds",FCVAR_NOTIFY,true,120.0);
    cvarJBFS[Version] = CreateConVar("jbfs_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);
    AutoExecConfig(true,"JBFS");

    //regular commands for players
    RegConsoleCmd("sm_w",Command_Warden,"Become the Warden");
    RegConsoleCmd("sm_warden",Command_Warden,"Become the Warden");
    //warden commands
    RegConsoleCmd("sm_uw",Command_UnWarden,"Retire from Warden");
    RegConsoleCmd("sm_unwarden",Command_UnWarden,"Retire from Warden");
    RegConsoleCmd("sm_oc",Command_OpenCells,"Open the cell doors");
    RegConsoleCmd("sm_opencells",Command_OpenCells,"Open the cell doors");
    RegConsoleCmd("sm_cc",Command_CloseCells,"Close the cell doors");
    RegConsoleCmd("sm_closecells",Command_CloseCells,"Close the cell doors");

    //admin commands
    RegAdminCmd("sm_fw",Command_Admin_ForceWarden,ADMFLAG_GENERIC,"Force a player to become Warden");
    RegAdminCmd("sm_forcewarden",Command_Admin_ForceWarden,ADMFLAG_GENERIC,"Force a player to become Warden");
    RegAdminCmd("sm_fuw",Command_Admin_ForceUnWarden,ADMFLAG_GENERIC,"Force the current Warden to retire");
    RegAdminCmd("sm_forceunwarden",Command_Admin_ForceUnWarden,ADMFLAG_GENERIC,"Force the current Warden to retire");
    RegAdminCmd("sm_lw",Command_Admin_LockWarden,ADMFLAG_GENERIC,"Lock Warden");
    RegAdminCmd("sm_lockwarden",Command_Admin_LockWarden,ADMFLAG_GENERIC,"Lock Warden");
    RegAdminCmd("sm_ulw",Command_Admin_UnlockWarden,ADMFLAG_GENERIC,"Unlock Warden");
    RegAdminCmd("sm_unlockwarden",Command_Admin_UnlockWarden,ADMFLAG_GENERIC,"Unlock Warden");
    RegAdminCmd("sm_jtime",Command_Admin_JailTime,ADMFLAG_GENERIC,"Set time left in round, in seconds");
    RegAdminCmd("sm_jailtime",Command_Admin_JailTime,ADMFLAG_GENERIC,"Set time left in round, in seconds");
    RegAdminCmd("sm_foc",Command_Admin_OpenCells,ADMFLAG_GENERIC,"Force open the cell doors");
    RegAdminCmd("sm_forceopencells",Command_Admin_OpenCells,ADMFLAG_GENERIC,"Force open the cell doors");
    RegAdminCmd("sm_fcc",Command_Admin_CloseCells,ADMFLAG_GENERIC,"Force close the cell doors");
    RegAdminCmd("sm_forceclosecells",Command_Admin_CloseCells,ADMFLAG_GENERIC,"Force close the cell doors");

    //hook gameevents for use as functions
    HookEvent("teamplay_round_start",OnPreRoundStart);
    HookEvent("arena_round_start",OnArenaRoundStart);
    HookEvent("player_disconnect",OnPlayerDisconnect);
    HookEvent("player_death",OnPlayerDeath);
    HookEvent("player_spawn",OnPlayerSpawn);
    HookEvent("teamplay_round_win",OnArenaRoundEnd);
    HookEvent("teamplay_round_stalemate",OnArenaRoundEnd);

    //create custom game events for vscript in maps to hook off of
    //we can't actually make entirely custom ones, BUT we can abuse existing events
    //in the case of this plugin, pass-time events will be used
    JBFSEvents.plugin_load = CreateEvent("pass_get",true);
    JBFSEvents.player_warden_set = CreateEvent("pass_free",true);
    JBFSEvents.player_warden_unset = CreateEvent("pass_stolen",true);

    SetConVars(true);

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);

    //import translations
    LoadTranslations("common.phrases");
    LoadTranslations("jbfs.phrases");

    //sounds to precache
    ManagePrecache();

    //various plugin configs
    LoadConfigs();
}

public void OnPluginEnd()
{
    SetConVars(false);
}

public void ManagePrecache()
{
    PrecacheSound("vo/announcer_ends_60sec.mp3", true);
    PrecacheSound("vo/announcer_ends_30sec.mp3", true);
    PrecacheSound("vo/announcer_ends_10sec.mp3", true);
    char sound[PLATFORM_MAX_PATH];
    for(int i=1;i<6;i++)
    {
        FormatEx(sound,PLATFORM_MAX_PATH,"vo/announcer_ends_%dsec.mp3",i);
        PrecacheSound(sound,true);
    }
}