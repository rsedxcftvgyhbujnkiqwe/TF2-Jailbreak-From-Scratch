#define PLUGIN_NAME         "Jailbreak From Scratch"
#define PLUGIN_VERSION      "0.1"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Minimal TF2 Jailbreak plugin"

//sourcemod incs
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
//jbfs incs
#include <JBFS/jbfs_vars>
#include <JBFS/jbfs_events>
#include <JBFS/jbfs_commands>
#include <JBFS/jbfs_stocks>
#include <JBFS/jbfs_timers>
#include <JBFS/jbfs_cfg>
#include <JBFS/jbfs_menu>
#include <JBFS/jbfs_vscript>
#include <JBFS/stocks>

//third party deps
#include <morecolors>
#undef REQUIRE_PLUGIN
#tryinclude <sourcecomms>
#tryinclude <vscript>
#define REQUIRE_PLUGIN

public void OnPluginStart()
{
    PrintToServer("Starting %s, version %s",PLUGIN_NAME,PLUGIN_VERSION);
    //register cvars
    cvarJBFS[BalanceRatio] = CreateConVar("sm_jbfs_balanceratio","0.5","Default balance ratio of blues to reds.",FCVAR_NOTIFY,true,0.1,true,1.0);
    cvarJBFS[TextChannel] = CreateConVar("sm_jbfs_textchannel","4","Default text channel for JBFS Hud text.",FCVAR_NOTIFY,true,0.0,true,5.0);
    cvarJBFS[GuardCrits] = CreateConVar("sm_jbfs_guardcrits","1","Should Guards have crits.\n0 = No Crits\n1 = Crits",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[RoundTime] = CreateConVar("sm_jbfs_roundtime","600","Time per round, in seconds",FCVAR_NOTIFY,true,120.0);
    cvarJBFS[WardayTime] = CreateConVar("sm_jbfs_wardaytime","300","Time per round on Warday, in seconds",FCVAR_NOTIFY,true,120.0);
    cvarJBFS[FreedayTime] = CreateConVar("sm_jbfs_freedaytime","300","Time per round on Freeday for all, in seconds",FCVAR_NOTIFY,true,120.0);
    cvarJBFS[MicBalance] = CreateConVar("sm_jbfs_micblanace","1","Whether to check for guard mics when autobalancing.\nGuards without a mic are autobalanced first.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.1,true,1.0)
    cvarJBFS[MicWarden] = CreateConVar("sm_jbfs_micwarden","1","Whether to check for guard mics when assigning warden.\nGuards without a mic are not allowed to be warden.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.1,true,1.0)
    cvarJBFS[FireVote] = CreateConVar("sm_jbfs_firevote","1","Enable the firing system, allowing prisoners to fire the warden by popular vote.\n0 = Disabled\n1 = Enabled",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[GuardFire] = CreateConVar("sm_jbfs_guardfire","1","If the firing system is enalbed, allow guards to participate in fire votes\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[FireRatio] = CreateConVar("sm_jbfs_fireratio","0.7","Ratio of eligible players required to fire the Warden.\nIf guard firing is enabled, guards are included in the ratio.",FCVAR_NOTIFY,true,0.1,true,1.0)
    cvarJBFS[MarkerTime] = CreateConVar("sm_jbfs_markertime","10.0","Timed life of markers.\n0 disables marker system.",FCVAR_NOTIFY,true,0.0,true,20.0);
    cvarJBFS[MarkerRadius] = CreateConVar("sm_jbfs_markerradius","256","Radius of markers, in hammer units.",FCVAR_NOTIFY,true,32.0,true,512.0);
    cvarJBFS[MarkerColor] = CreateConVar("sm_jbfs_markercolor","FFFFFF","Hex color to use for markers.",FCVAR_NOTIFY);
    cvarJBFS[Version] = CreateConVar("jbfs_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);
    //admincmd cvars
    cvarJBFS_ACMD[ACMD_WardenMenu] = CreateConVar("sm_jbfs_acmd_adminmenu","2","Admin command section. Requires setting admin flag bits.\nSee: https://wiki.alliedmods.net/Checking_Admin_Flags_(SourceMod_Scripting)\n\nAdmin flag(s) required to open the admin warden menu.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_ACMD[ACMD_ForceWarden] = CreateConVar("sm_jbfs_acmd_forcewarden","2","Admin flag(s) required to force warden/unwarden.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_ACMD[ACMD_LockWarden] = CreateConVar("sm_jbfs_acmd_lockwarden","2","Admin flag(s) required to lock/unlock warden.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_ACMD[ACMD_JailTime] = CreateConVar("sm_jbfs_acmd_jailtime","2","Admin flag(s) required to change jail time.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_ACMD[ACMD_Cells] = CreateConVar("sm_jbfs_acmd_cells","2","Admin flag(s) required to open/close cells.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_ACMD[ACMD_FF] = CreateConVar("sm_jbfs_acmd_ff","2","Admin flag(s) required to toggle friendly fire.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_ACMD[ACMD_CC] = CreateConVar("sm_jbfs_acmd_cc","2","Admin flag(s) required to toggle collisions.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    AutoExecConfig(true,"JBFS");

    //regular commands for players
    RegConsoleCmd("sm_w",Command_Warden,"Become the Warden");
    RegConsoleCmd("sm_warden",Command_Warden,"Become the Warden");
    RegConsoleCmd("sm_ff",Command_FriendlyFireStatus,"Show status of Friendly Fire");
    RegConsoleCmd("sm_fire",Command_FireWarden,"Vote to fire the Warden");

    //warden commands
    RegConsoleCmd("sm_uw",Command_UnWarden,"Retire from Warden");
    RegConsoleCmd("sm_unwarden",Command_UnWarden,"Retire from Warden");
    RegConsoleCmd("sm_oc",Command_OpenCells,"Open the cell doors");
    RegConsoleCmd("sm_opencells",Command_OpenCells,"Open the cell doors");
    RegConsoleCmd("sm_cc",Command_CloseCells,"Close the cell doors");
    RegConsoleCmd("sm_closecells",Command_CloseCells,"Close the cell doors");
    RegConsoleCmd("sm_wm",Command_WardenMenu,"Open the Warden menu");
    RegConsoleCmd("sm_wmenu",Command_WardenMenu,"Open the Warden menu");
    RegConsoleCmd("sm_wardenmenu",Command_WardenMenu,"Open the Warden menu");
    RegConsoleCmd("sm_wff",Command_ToggleFriendlyFire,"Toggle Friendly Fire");
    RegConsoleCmd("sm_wardenff",Command_ToggleFriendlyFire,"Toggle Friendly Fire");
    RegConsoleCmd("sm_wcc",Command_ToggleCollisions,"Toggle Collisions");
    RegConsoleCmd("sm_wcol",Command_ToggleCollisions,"Toggle Collisions");
    RegConsoleCmd("sm_wardencol",Command_ToggleCollisions,"Toggle Collisions");
    RegConsoleCmd("sm_marker",Command_WardenMarker,"Create a Warden marker");
    RegConsoleCmd("sm_glr",Command_GiveLastRequest,"Give a prisoner LR");
    RegConsoleCmd("sm_givelr",Command_GiveLastRequest,"Give a prisoner LR");
    //admin commands
    RegAdminCmd("sm_fw",Command_Admin_ForceWarden,cvarJBFS_ACMD[ACMD_ForceWarden].IntValue,"Force a player to become Warden");
    RegAdminCmd("sm_forcewarden",Command_Admin_ForceWarden,cvarJBFS_ACMD[ACMD_ForceWarden].IntValue,"Force a player to become Warden");
    RegAdminCmd("sm_fuw",Command_Admin_ForceUnWarden,cvarJBFS_ACMD[ACMD_ForceWarden].IntValue,"Force the current Warden to retire");
    RegAdminCmd("sm_forceretire",Command_Admin_ForceUnWarden,cvarJBFS_ACMD[ACMD_ForceWarden].IntValue,"Force the current Warden to retire");
    RegAdminCmd("sm_forceunwarden",Command_Admin_ForceUnWarden,cvarJBFS_ACMD[ACMD_ForceWarden].IntValue,"Force the current Warden to retire");
    RegAdminCmd("sm_lw",Command_Admin_LockWarden,cvarJBFS_ACMD[ACMD_LockWarden].IntValue,"Lock Warden");
    RegAdminCmd("sm_lockwarden",Command_Admin_LockWarden,cvarJBFS_ACMD[ACMD_LockWarden].IntValue,"Lock Warden");
    RegAdminCmd("sm_ulw",Command_Admin_UnlockWarden,cvarJBFS_ACMD[ACMD_LockWarden].IntValue,"Unlock Warden");
    RegAdminCmd("sm_unlockwarden",Command_Admin_UnlockWarden,cvarJBFS_ACMD[ACMD_LockWarden].IntValue,"Unlock Warden");
    RegAdminCmd("sm_jtime",Command_Admin_JailTime,cvarJBFS_ACMD[ACMD_JailTime].IntValue,"Set time left in round, in seconds");
    RegAdminCmd("sm_jailtime",Command_Admin_JailTime,cvarJBFS_ACMD[ACMD_JailTime].IntValue,"Set time left in round, in seconds");
    RegAdminCmd("sm_foc",Command_Admin_OpenCells,cvarJBFS_ACMD[ACMD_Cells].IntValue,"Force open the cell doors");
    RegAdminCmd("sm_forceopencells",Command_Admin_OpenCells,cvarJBFS_ACMD[ACMD_Cells].IntValue,"Force open the cell doors");
    RegAdminCmd("sm_fcc",Command_Admin_CloseCells,cvarJBFS_ACMD[ACMD_Cells].IntValue,"Force close the cell doors");
    RegAdminCmd("sm_forceclosecells",Command_Admin_CloseCells,cvarJBFS_ACMD[ACMD_Cells].IntValue,"Force close the cell doors");
    RegAdminCmd("sm_aff",Command_Admin_ToggleFriendlyFire,cvarJBFS_ACMD[ACMD_FF].IntValue,"Toggle Friendly Fire");
    RegAdminCmd("sm_adminff",Command_Admin_ToggleFriendlyFire,cvarJBFS_ACMD[ACMD_FF].IntValue,"Toggle Friendly Fire");
    RegAdminCmd("sm_acc",Command_Admin_ToggleCollisions,cvarJBFS_ACMD[ACMD_CC].IntValue,"Toggle Collisions");
    RegAdminCmd("sm_acol",Command_Admin_ToggleCollisions,cvarJBFS_ACMD[ACMD_CC].IntValue,"Toggle Collisions");
    RegAdminCmd("sm_admincol",Command_Admin_ToggleCollisions,cvarJBFS_ACMD[ACMD_CC].IntValue,"Toggle Collisions");
    RegAdminCmd("sm_flr",Command_Admin_ForceLastRequest,ADMFLAG_GENERIC,"Force give a prisoner LR");
    RegAdminCmd("sm_forcelr",Command_Admin_ForceLastRequest,ADMFLAG_GENERIC,"Force give a prisoner LR");

    RegAdminCmd("sm_awm",Command_Admin_WardenMenu,cvarJBFS_ACMD[ACMD_WardenMenu].IntValue,"Open the Admin Warden menu");
    RegAdminCmd("sm_awmenu",Command_Admin_WardenMenu,cvarJBFS_ACMD[ACMD_WardenMenu].IntValue,"Open the Admin Warden menu");

    //hook gameevents for use as functions
    HookEvent("teamplay_round_start",OnPreRoundStart);
    HookEvent("arena_round_start",OnArenaRoundStart);
    HookEvent("player_disconnect",OnPlayerDisconnect);
    HookEvent("player_death",OnPlayerDeath);
    HookEvent("player_spawn",OnPlayerSpawn);
    HookEvent("teamplay_round_win",OnArenaRoundEnd);
    HookEvent("teamplay_round_stalemate",OnArenaRoundEnd);
    HookEvent("player_team",OnPlayerTeamChange);

    SetConVars(true);

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);

    //import translations
    LoadTranslations("common.phrases");
    LoadTranslations("jbfs/jbfs.phrases");
    LoadTranslations("jbfs/jbfs.menu");

    //sounds to precache
    ManagePrecache();
}

public void OnMapStart()
{
    //various plugin configs
    LoadConfigs();

    //make sure LRs dont carry-over
    ResetLR();
    ResetActiveLR();

    //vscript
#if defined _vscript_included
    RegisterVScriptFunctions();
#endif
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
    
    JBMarker.BeamSprite = PrecacheModel("materials/sprites/laserbeam.vmt", true);
    JBMarker.HaloSprite = PrecacheModel("materials/sprites/halo01.vmt", true);
    cvarJBFS[MarkerColor].GetString(JBMarker.MarkerColorHex,sizeof(JBMarker.MarkerColorHex))
    int rgb[3];
    HexToRGB(JBMarker.MarkerColorHex,rgb);
    for(int i;i<3;i++){
        JBMarker.MarkerColorRGBA[i] = rgb[i];
    }
}

public void OnAllPluginsLoaded()
{
#if defined _sourcecomms_included
    sourcecommspp = LibraryExists("sourcecomms++");
#endif
#if defined _vscript_included
    vscript = LibraryExists("vscript");
    if(vscript)
        //vscript
        CreateVScriptFunctions()
#endif
}
 
public void OnLibraryRemoved(const char[] name)
{
#if defined _sourcecomms_included
    if (StrEqual(name, "sourcecomms++"))
    {
        sourcecommspp = false;
    }
#endif
}
 
public void OnLibraryAdded(const char[] name)
{
#if defined _sourcecomms_included
    if (StrEqual(name, "sourcecomms++"))
    {
        sourcecommspp = true;
    }
#endif
}