#define PLUGIN_NAME         "Jailbreak From Scratch"
#define PLUGIN_VERSION      "1.2"
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
#include <JBFS/jbfs_natives>
#include <JBFS/stocks>

//third party deps
#include <morecolors>
#include <tf2utils>
#include <tf2attributes>
#undef REQUIRE_PLUGIN
#tryinclude <sourcecomms>
#tryinclude <vscript>
#define REQUIRE_PLUGIN

#include <JBFS/jbfs_vscript>
#include <jbfs>

public void OnPluginStart()
{
    PrintToServer("===Starting %s, version %s===",PLUGIN_NAME,PLUGIN_VERSION);
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
    cvarJBFS[GuardFire] = CreateConVar("sm_jbfs_guardfire","1","If the firing system is enabled, allow guards to participate in fire votes\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[FireRatio] = CreateConVar("sm_jbfs_fireratio","0.7","Ratio of eligible players required to fire the Warden.\nIf guard firing is enabled, guards are included in the ratio.",FCVAR_NOTIFY,true,0.1,true,1.0)
    cvarJBFS[MarkerTime] = CreateConVar("sm_jbfs_markertime","10.0","Timed life of markers.",FCVAR_NOTIFY,true,1.0,true,20.0);
    cvarJBFS[MarkerRadius] = CreateConVar("sm_jbfs_markerradius","256","Radius of markers, in hammer units.",FCVAR_NOTIFY,true,32.0,true,512.0);
    cvarJBFS[MarkerColor] = CreateConVar("sm_jbfs_markercolor","FFFFFF","Hex color to use for markers.",FCVAR_NOTIFY);
    cvarJBFS[RemoveItem1] = CreateConVar("sm_jbfs_removeitemone","1","Should Item1 be removed from all players?.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[RemoveItem2] = CreateConVar("sm_jbfs_removeitemtwo","1","Should Item2 be removed from all players?.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[ReplaceWeapon] = CreateConVar("sm_jbfs_replaceweapon","1","For warday/all banned weapons, should they be replaced with stock when removed?\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[RedBuild] = CreateConVar("sm_jbfs_buildred","0","Buildings that Red engineers can build (with ammo + PDA) on non-warday.\nBit counter\n0=None\n1=Dispenser\n2=Teleporter\n4=Sentry\nExample: 0 = Nothing, 3 = Teleporters and Dispensers, 6 = Teleporters and Sentries",FCVAR_NOTIFY,true,0.0,true,7.0);
    cvarJBFS[BlueBuild] = CreateConVar("sm_jbfs_buildblue","0","Buildings that Blue engineers can build (with ammo + PDA) on non-warday.\nBit counter\n0=None\n1=Dispenser\n2=Teleporter\n4=Sentry\nExample: 0 = Nothing, 3 = Teleporters and Dispensers, 6 = Teleporters and Sentries",FCVAR_NOTIFY,true,0.0,true,7.0);
    cvarJBFS[RedBuildWarday] = CreateConVar("sm_jbfs_wardaybuildred","1","Can Red Engineers build all buildings on Warday?\n0 = No, use sm_jbfs_buildred build list\n1 = Yes",FCVAR_NOTIFY,true,0.1,true,1.0);
    cvarJBFS[BlueBuildWarday] = CreateConVar("sm_jbfs_wardaybuildblue","1","Can Blue Engineers build all buildings on Warday?\n0 = No, use sm_jbfs_buildblue build list\n1 = Yes",FCVAR_NOTIFY,true,0.1,true,1.0);
    cvarJBFS[DroppedWeapon] = CreateConVar("sm_jbfs_killdroppedweapons","1","Kill dropped weapons?\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[DroppedAmmo] = CreateConVar("sm_jbfs_killdroppedammo","1","Kill dropped ammo boxes?\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[PointServerCMD] = CreateConVar("sm_jbfs_killpointservercmd","1","Kill point_servercommand entities?\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[DoubleJump] = CreateConVar("sm_jbfs_doublejump","1","Can scouts double jump?\n0 = No\n1 = With ammo (blues by default)\n2 = Blues only\n3 = Yes (all)",FCVAR_NOTIFY,true,0.0,true,3.0);
    cvarJBFS[AirblastImmunity] = CreateConVar("sm_jbfs_airblastimmunity","1","Should players have airblast push immunity?\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0);
    cvarJBFS[Disguising] = CreateConVar("sm_jbfs_disguising","0","Can spies disguise?\nRequires disguise kit\n0 = No\n1 = With ammo (blues by default)\n2 = Blues only\n3 = Yes (all)",FCVAR_NOTIFY,true,0.0,true,3.0);
    cvarJBFS[DemoCharge] = CreateConVar("sm_jbfs_democharge","1","Can demomen charge?\n0 = No\n1 = With ammo (blues by default)\n2 = Blues only\n3 = Yes (all)",FCVAR_NOTIFY,true,0.0,true,3.0);
    cvarJBFS[LRSetTime] = CreateConVar("sm_jbfs_lrtime","0","Round timer will be set to this many seconds once LR is given.\n0 disables the check",FCVAR_NOTIFY,true,0.0,true,300.0);
    cvarJBFS[LastRequest] = CreateConVar("sm_jbfs_lastrequest","1","Enable the Last Request system.\nDisabling it will prevent wardens and admins from giving out last request to prisoners.\n0 = Disabled\n1 = Enabled",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[Version] = CreateConVar("jbfs_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);
    //admincmd cvars
    cvarJBFS_CMD[ACMD_WardenMenu] = CreateConVar("sm_jbfs_acmd_adminmenu","2","Admin commands (sm_jbfs_acmd_*) requires setting admin flag bits.\nSee: https://wiki.alliedmods.net/Checking_Admin_Flags_(SourceMod_Scripting)\n\nAdmin flag(s) required to open the admin warden menu.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_ForceWarden] = CreateConVar("sm_jbfs_acmd_forcewarden","2","Admin flag(s) required to force warden/unwarden.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_LockWarden] = CreateConVar("sm_jbfs_acmd_lockwarden","2","Admin flag(s) required to lock/unlock warden.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_JailTime] = CreateConVar("sm_jbfs_acmd_jailtime","2","Admin flag(s) required to change jail time.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_Cells] = CreateConVar("sm_jbfs_acmd_cells","2","Admin flag(s) required to open/close cells.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_FF] = CreateConVar("sm_jbfs_acmd_ff","2","Admin flag(s) required to toggle friendly fire.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_CC] = CreateConVar("sm_jbfs_acmd_cc","2","Admin flag(s) required to toggle collisions.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_ForceLR] = CreateConVar("sm_jbfs_acmd_forcelr","2","Admin flag(s) required to force last request.",FCVAR_NOTIFY,true,0.0,true,2097151.0);
    cvarJBFS_CMD[ACMD_ForceFreeday] = CreateConVar("sm_jbfs_acmd_forcefreeday","2","Admin flag(s) required to force freeday.",FCVAR_NOTIFY,true,0.0,true,2097151.0)
    //wmenu cvars
    cvarJBFS_CMD[WCMD_Cells] = CreateConVar("sm_jbfs_wcmd_cells","1","Allow the Warden to open/close the cell doors\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS_CMD[WCMD_FriendlyFire] = CreateConVar("sm_jbfs_wcmd_ff","1","Allow the Warden to toggle friendly-fire\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS_CMD[WCMD_Collisions] = CreateConVar("sm_jbfs_wcmd_cc","1","Allow the Warden to toggle collisions\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS_CMD[WCMD_Marker] = CreateConVar("sm_jbfs_wcmd_marker","1","Allow the Warden to create markers\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    AutoExecConfig(true,"jbfs");

    //regular commands for players
    RegConsoleCmd("sm_w",Command_Warden,"Become the Warden");
    RegConsoleCmd("sm_warden",Command_Warden,"Become the Warden");
    RegConsoleCmd("sm_ff",Command_FriendlyFireStatus,"Show status of Friendly Fire");
    RegConsoleCmd("sm_fire",Command_FireWarden,"Vote to fire the Warden");
    RegConsoleCmd("sm_lr",Command_CheckLastRequest,"Check the current Last Request");

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


    //hook gameevents for use as functions
    HookEvent("teamplay_round_start",OnPreRoundStart);
    HookEvent("arena_round_start",OnArenaRoundStart);
    HookEvent("player_disconnect",OnPlayerDisconnect);
    HookEvent("player_death",OnPlayerDeath);
    HookEvent("player_spawn",OnPlayerSpawn);
    HookEvent("teamplay_round_win",OnArenaRoundEnd);
    HookEvent("teamplay_round_stalemate",OnArenaRoundEnd);
    HookEvent("player_team",OnPlayerTeamChange);

    HookEntityOutput("item_ammopack_full", "OnCacheInteraction", OnTakeAmmo);
    HookEntityOutput("item_ammopack_medium", "OnCacheInteraction", OnTakeAmmo);
    HookEntityOutput("item_ammopack_small", "OnCacheInteraction", OnTakeAmmo);
    HookEntityOutput("tf_ammo_pack", "OnCacheInteraction", OnTakeAmmo);

    SetConVars(true);

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);

    //import translations
    LoadTranslations("common.phrases");
    LoadTranslations("jbfs/jbfs.phrases");
    LoadTranslations("jbfs/jbfs.menu");
}

public void OnConfigsExecuted()
{
    //admin commands
    RegAdminCmd("sm_fw",Command_Admin_ForceWarden,cvarJBFS_CMD[ACMD_ForceWarden].IntValue,"Force a player to become Warden");
    RegAdminCmd("sm_forcewarden",Command_Admin_ForceWarden,cvarJBFS_CMD[ACMD_ForceWarden].IntValue,"Force a player to become Warden");
    RegAdminCmd("sm_fuw",Command_Admin_ForceUnWarden,cvarJBFS_CMD[ACMD_ForceWarden].IntValue,"Force the current Warden to retire");
    RegAdminCmd("sm_forceretire",Command_Admin_ForceUnWarden,cvarJBFS_CMD[ACMD_ForceWarden].IntValue,"Force the current Warden to retire");
    RegAdminCmd("sm_forceunwarden",Command_Admin_ForceUnWarden,cvarJBFS_CMD[ACMD_ForceWarden].IntValue,"Force the current Warden to retire");
    RegAdminCmd("sm_lw",Command_Admin_LockWarden,cvarJBFS_CMD[ACMD_LockWarden].IntValue,"Lock Warden");
    RegAdminCmd("sm_lockwarden",Command_Admin_LockWarden,cvarJBFS_CMD[ACMD_LockWarden].IntValue,"Lock Warden");
    RegAdminCmd("sm_ulw",Command_Admin_UnlockWarden,cvarJBFS_CMD[ACMD_LockWarden].IntValue,"Unlock Warden");
    RegAdminCmd("sm_unlockwarden",Command_Admin_UnlockWarden,cvarJBFS_CMD[ACMD_LockWarden].IntValue,"Unlock Warden");
    RegAdminCmd("sm_jt",Command_Admin_JailTime,cvarJBFS_CMD[ACMD_JailTime].IntValue,"Set time left in round, in seconds");
    RegAdminCmd("sm_jtime",Command_Admin_JailTime,cvarJBFS_CMD[ACMD_JailTime].IntValue,"Set time left in round, in seconds");
    RegAdminCmd("sm_jailtime",Command_Admin_JailTime,cvarJBFS_CMD[ACMD_JailTime].IntValue,"Set time left in round, in seconds");
    RegAdminCmd("sm_foc",Command_Admin_OpenCells,cvarJBFS_CMD[ACMD_Cells].IntValue,"Force open the cell doors");
    RegAdminCmd("sm_forceopencells",Command_Admin_OpenCells,cvarJBFS_CMD[ACMD_Cells].IntValue,"Force open the cell doors");
    RegAdminCmd("sm_fcc",Command_Admin_CloseCells,cvarJBFS_CMD[ACMD_Cells].IntValue,"Force close the cell doors");
    RegAdminCmd("sm_forceclosecells",Command_Admin_CloseCells,cvarJBFS_CMD[ACMD_Cells].IntValue,"Force close the cell doors");
    RegAdminCmd("sm_aff",Command_Admin_ToggleFriendlyFire,cvarJBFS_CMD[ACMD_FF].IntValue,"Toggle Friendly Fire");
    RegAdminCmd("sm_adminff",Command_Admin_ToggleFriendlyFire,cvarJBFS_CMD[ACMD_FF].IntValue,"Toggle Friendly Fire");
    RegAdminCmd("sm_acc",Command_Admin_ToggleCollisions,cvarJBFS_CMD[ACMD_CC].IntValue,"Toggle Collisions");
    RegAdminCmd("sm_acol",Command_Admin_ToggleCollisions,cvarJBFS_CMD[ACMD_CC].IntValue,"Toggle Collisions");
    RegAdminCmd("sm_admincol",Command_Admin_ToggleCollisions,cvarJBFS_CMD[ACMD_CC].IntValue,"Toggle Collisions");
    RegAdminCmd("sm_flr",Command_Admin_ForceLastRequest,cvarJBFS_CMD[ACMD_ForceLR].IntValue,"Force give a prisoner LR");
    RegAdminCmd("sm_forcelr",Command_Admin_ForceLastRequest,cvarJBFS_CMD[ACMD_ForceLR].IntValue,"Force give a prisoner LR");
    RegAdminCmd("sm_freeday",Command_Admin_ForceFreeday,cvarJBFS_CMD[ACMD_ForceFreeday].IntValue,"Force give a prisoner a freeday")
    RegAdminCmd("sm_awm",Command_Admin_WardenMenu,cvarJBFS_CMD[ACMD_WardenMenu].IntValue,"Open the Admin Warden menu");
    RegAdminCmd("sm_awmenu",Command_Admin_WardenMenu,cvarJBFS_CMD[ACMD_WardenMenu].IntValue,"Open the Admin Warden menu");
}

public void OnMapStart()
{
    //various plugin configs
    LoadConfigs();

    //sounds to precache
    ManagePrecache();

    //make sure stuff doesn't carry over
    ResetLR();
    LRConfig.LRChosen = LR_None;

    //vscript
#if defined _vscript_included
    if(vscript)
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
    PrecacheSound("misc/rd_finale_beep01.wav", true);


    JBMarker.BeamSprite = PrecacheModel("materials/sprites/laserbeam.vmt", true);
    JBMarker.HaloSprite = PrecacheModel("materials/sprites/halo01.vmt", true);
    cvarJBFS[MarkerColor].GetString(JBMarker.MarkerColorHex,sizeof(JBMarker.MarkerColorHex))
    int rgb[3];
    HexToRGB(JBMarker.MarkerColorHex,rgb);
    for(int i;i<3;i++){
        JBMarker.MarkerColorRGBA[i] = rgb[i];
    }
    JBMarker.MarkerColorRGBA[3] = 255;
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
#if defined _vscript_included
    if (StrEqual(name, "vscript"))
    {
        vscript = false;
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
#if defined _vscript_included
    if (StrEqual(name, "vscript"))
    {
        vscript = true;
    }
#endif
}

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
    CreateNative("JBFS_AddGuardBan",Native_AddGuardBan);
    CreateNative("JBFS_RemoveGuardBan",Native_RemoveGuardBan);
    CreateNative("JBFS_IsGuardBanned",Native_IsGuardBanned);

    RegPluginLibrary("JailbreakFromScratch");
    return APLRes_Success;
}