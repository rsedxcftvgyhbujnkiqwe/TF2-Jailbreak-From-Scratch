#define PLUGIN_NAME         "[JBFS] Gangs"
#define PLUGIN_VERSION      "1.0"
#define PLUGIN_AUTHOR       "blank"
#define PLUGIN_DESCRIPTION  "Gang plugin for JBFS"

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

#include <morecolors>

//jbfs incs
#include <JBFS/jbfsg_vars>
#include <JBFS/jbfsg_database>
#include <JBFS/jbfsg_commands>
#include <JBFS/jbfsg_stocks>

public void OnPluginStart()
{
    cvarJBFS[DatabaseName] = CreateConVar("sm_jbfsg_database","jbfsgangs","Name of SQL database to use. Configured in databases.cfg",FCVAR_NOTIFY)
    cvarJBFS[AnnounceGangCreate] = CreateConVar("sm_jbfsg_announcegangcreate","1","Whether to announce to all players when gangs are created/abandoned.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[AnnounceGangName] = CreateConVar("sm_jbfsg_announcegangname","1","Whether to announce to all players when gangs change their names.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[GangChat] = CreateConVar("sm_jbfsg_gangchat","1","Enable the Gang chat feature, which allows gang members to send messages only other gang members can see.\n0 = No\n1 = Yes",FCVAR_NOTIFY,true,0.0,true,1.0)
    cvarJBFS[MaxGangSize] = CreateConVar("sm_jbfs_maxgangsize","12","Maximum number of players allowed in a gang.\nReducing this later will not prune members",FCVAR_NOTIFY,true,1.0,true,64.0)
    cvarJBFS[Version] = CreateConVar("jbfst_version",PLUGIN_VERSION,PLUGIN_NAME,FCVAR_REPLICATED | FCVAR_NOTIFY | FCVAR_SPONLY | FCVAR_DONTRECORD);
    AutoExecConfig(true,"jbfsgangs");

    //add custom color(s) to morecolors
    CCheckTrie();
    SetTrieValue(CTrie,"day9",0xFFA71A);

    LoadTranslations("common.phrases");
    LoadTranslations("jbfs/jbfs.phrases");
    LoadTranslations("jbfs/jbfs.gangs");

    //generic cmds
    //RegConsoleCmd("sm_ganghelp",Command_GangHelp,"Help menu for Gang related commands")
    RegConsoleCmd("sm_gangcreate",Command_CreateGang,"Create a gang with the specified name.");
    RegConsoleCmd("sm_gangjoin",Command_GangJoin,"Join the gang you have been invited to.");
    RegConsoleCmd("sm_ganghelp",Command_GangHelp,"Display a help menu for gang commands.")

    //gang cmds
    RegConsoleCmd("sm_gangleave",Command_GangLeave,"Leave your gang. If sole member, disband gang.");
    RegConsoleCmd("sm_gangchat",Command_GangChat,"Send a message to your gang.");
    RegConsoleCmd("sm_gc",Command_GangChat,"Send a message to your gang.");
    RegConsoleCmd("sm_ganglist",Command_GangList,"List all members in the gang and their Member IDs.");


    //boss cmds
    RegConsoleCmd("sm_ganginvite",Command_GangInvite,"Invite a player to your gang.");
    RegConsoleCmd("sm_gangname",Command_SetGangName,"Change the name of your gang.");
    RegConsoleCmd("sm_gangtag",Command_SetGangTag,"Change your gang's tag.");
    RegConsoleCmd("sm_gangpromote",Command_GangPromote,"Promote a muscle to officer, or officer to boss.");
    RegConsoleCmd("sm_gangdemote",Command_GangDemote,"Demote an officer to muscle.");
    RegConsoleCmd("sm_gangkick",Command_GangKick,"Kick a member from your gang.");
}

public void OnConfigsExecuted()
{
    DBConnect();
}