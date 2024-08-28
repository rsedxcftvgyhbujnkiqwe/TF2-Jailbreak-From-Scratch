//I really do not enjoy doing menus

#define WMENU_CHOICE1 "#wmchoice1"
#define WMENU_CHOICE2 "#wmchoice2"
#define WMENU_CHOICE3 "#wmchoice3"
#define WMENU_CHOICE4 "#wmchoice4"
#define WMENU_CHOICE5 "#wmchoice5"
#define WMENU_CHOICE6 "#wmchoice6"
#define WMENU_CHOICE7 "#wmchoice7"
#define WMENU_CHOICE8 "#wmchoice8"
#define WMENU_CHOICE9 "#wmchoice9"


#define LRCHOICE1 "#lrchoice1"
#define LRCHOICE2 "#lrchoice2"
#define LRCHOICE3 "#lrchoice3"
#define LRCHOICE4 "#lrchoice4"
#define LRCHOICE5 "#lrchoice5"

public int WardenMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch(action)
    {
        case MenuAction_Start:
        {
            //do nothing for now
        }
        case MenuAction_Display:
        {
            char buffer[255];
            Format(buffer, sizeof(buffer), "%T", "WardenMenuTitle", param1);
            Panel panel = view_as<Panel>(param2);
            panel.SetTitle(buffer);
        }
        case MenuAction_Select:
        {
            //sanity check just in case someone somehow gets the warden menu opened when not warden
            if (param1 != JBFS.Warden)
            {
                delete menu;
                return 0;
            }
            float gametime = GetGameTime();
                //0.5s cooldown between picks
            if (gametime - g_WMenuTime < 0.5)
            {
                CreateWardenMenu(param1, menu.Selection)
                return 0;
            }

            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            if (StrEqual(info, WMENU_CHOICE1))
            {
                ModifyCells(true,param1);
            }
            else if (StrEqual(info, WMENU_CHOICE2))
            {
                ModifyCells(false,param1);
            }
            else if (StrEqual(info, WMENU_CHOICE3))
            {
                ToggleFF(false);
            }
            else if (StrEqual(info, WMENU_CHOICE4))
            {
                ToggleCC(false);
            }
            else if (StrEqual(info, WMENU_CHOICE5))
            {
                CreateMarker(param1);
            }
            else if (StrEqual(info, WMENU_CHOICE6))
            {
                CPrintToChat(param1,"%T %T","PluginTag","CommandUsageGiveLR")
            }
            else if (StrEqual(info, WMENU_CHOICE7))
            {
                Command_WardenMute(param1,0);
            }
            g_WMenuTime = gametime;
            CreateWardenMenu(param1, menu.Selection)
        }
        case MenuAction_Cancel:
        {
            g_WMenu[param1] = null;
            switch(param2)
            {
                case MenuCancel_Exit,MenuCancel_NoDisplay:
                {
                    CPrintToChat(param1,"%T %T","PluginTag","WardenMenuReOpen");
                }
            }
        }
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_DrawItem:
        {
            //do nothing
        }
        case MenuAction_DisplayItem:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            char display[64];
            if (StrEqual(info,WMENU_CHOICE1))
            {
                Format(display,sizeof(display),"%T","WardenMenuOpenCells",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE2))
            {
                Format(display,sizeof(display),"%T","WardenMenuCloseCells",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE3))
            {
                Format(display,sizeof(display),"%T","WardenMenuToggleFF",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE4))
            {
                Format(display,sizeof(display),"%T","WardenMenuToggleCC",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE5))
            {
                Format(display,sizeof(display),"%T","WardenMenuMarker",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE6))
            {
                Format(display,sizeof(display),"%T","WardenMenuLastRequest",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE7))
            {
                Format(display,sizeof(display),"%T","WardenMenuMute",param1);
                return RedrawMenuItem(display);
            }
        }
    }
    return 0;
}

public Action Command_WardenMenu(int client, int args)
{
    if(client!=JBFS.Warden)
    {
        CPrintToChat(client,"%T %T","PluginTag","WardenWarnNotWarden");
        return Plugin_Handled;
    }
    CreateWardenMenu(client,0);
    return Plugin_Handled;
}
//abstration done so this can be called from menu handler
public void CreateWardenMenu(int client,int selection)
{
    Menu menu = new Menu(WardenMenuHandler, MENU_ACTIONS_ALL);
    g_WMenu[client] = menu;
    menu.SetTitle("%T","WardenMenuTitle",client);
    if (cvarJBFS_CMD[WCMD_Cells].BoolValue)
    {
        menu.AddItem(WMENU_CHOICE1,"Open Cells");
        menu.AddItem(WMENU_CHOICE2,"Close Cells");
    }
    if (cvarJBFS_CMD[WCMD_FriendlyFire].BoolValue) menu.AddItem(WMENU_CHOICE3,"Toggle Friendly-Fire");
    if (cvarJBFS_CMD[WCMD_Collisions].BoolValue) menu.AddItem(WMENU_CHOICE4,"Toggle Collisions");
    if (cvarJBFS_CMD[WCMD_Marker].BoolValue) menu.AddItem(WMENU_CHOICE5,"Marker");
    menu.AddItem(WMENU_CHOICE6,"Last Request");
    if (cvarJBFS_CMD[WCMD_Mute].BoolValue) menu.AddItem(WMENU_CHOICE7,"Warden Mute");
    menu.DisplayAt(client,selection,MENU_TIME_FOREVER);
}

public int AdminWardenMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch(action)
    {
        case MenuAction_Start:
        {
            //do nothing for now
        }
        case MenuAction_Display:
        {
            char buffer[255];
            Format(buffer, sizeof(buffer), "%T", "WardenMenuTitle", param1);
            Panel panel = view_as<Panel>(param2);
            panel.SetTitle(buffer);
        }
        case MenuAction_Select:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            if (StrEqual(info, WMENU_CHOICE1))
            {
                Command_Admin_OpenCells(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE2))
            {
                Command_Admin_CloseCells(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE3))
            {
                Command_Admin_ToggleFriendlyFire(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE4))
            {
                Command_Admin_ToggleCollisions(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE5))
            {
                Command_Admin_ForceWarden(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE6))
            {
                Command_Admin_ForceUnWarden(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE7))
            {
                Command_Admin_LockWarden(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE8))
            {
                Command_Admin_UnlockWarden(param1,0);
            }
            else if (StrEqual(info, WMENU_CHOICE9))
            {
                Command_Admin_WardenMute(param1,0);
            }
            CreateAdminWardenMenu(param1, menu.Selection)
        }
        case MenuAction_Cancel:
        {
            //do nothing
        }
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_DrawItem:
        {
            //do nothing
        }
        case MenuAction_DisplayItem:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            char display[64];
            if (StrEqual(info,WMENU_CHOICE1))
            {
                Format(display,sizeof(display),"%T","WardenMenuOpenCells",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE2))
            {
                Format(display,sizeof(display),"%T","WardenMenuCloseCells",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE3))
            {
                Format(display,sizeof(display),"%T","WardenMenuToggleFF",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE4))
            {
                Format(display,sizeof(display),"%T","WardenMenuToggleCC",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE5))
            {
                Format(display,sizeof(display),"%T","AdminWardenMenuForceWarden",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE6))
            {
                Format(display,sizeof(display),"%T","AdminWardenMenuRetireWarden",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE7))
            {
                Format(display,sizeof(display),"%T","AdminWardenMenuLockWarden",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE8))
            {
                Format(display,sizeof(display),"%T","AdminWardenMenuUnlockWarden",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,WMENU_CHOICE9))
            {
                Format(display,sizeof(display),"%T","AdminWardenMenuWardenMute",param1);
                return RedrawMenuItem(display);
            }
        }
    }
    return 0;
}

public Action Command_Admin_WardenMenu(int client, int args)
{
    CreateAdminWardenMenu(client,0);
    return Plugin_Handled;
}
public void CreateAdminWardenMenu(int client,int selection)
{
    Menu menu = new Menu(AdminWardenMenuHandler,MENU_ACTIONS_ALL);
    menu.SetTitle("%T","WardenMenuTitle",client);
    if(CheckCommandAccess(client,"sm_forceopencells",2))
    {
        menu.AddItem(WMENU_CHOICE1,"Open Cells");
        menu.AddItem(WMENU_CHOICE2,"Close Cells");
    }
    if(CheckCommandAccess(client,"sm_adminff",2))
        menu.AddItem(WMENU_CHOICE3,"Toggle Friendly-Fire");
    if(CheckCommandAccess(client,"sm_admincc",2))
        menu.AddItem(WMENU_CHOICE4,"Toggle Collisions");
    if(CheckCommandAccess(client,"sm_forcewarden",2))
    {
        menu.AddItem(WMENU_CHOICE5,"Force Random Warden");
        menu.AddItem(WMENU_CHOICE6,"Force Warden to Retire");
    }
    if(CheckCommandAccess(client,"sm_lockwarden",2))
    {
        menu.AddItem(WMENU_CHOICE7,"Lock Warden");
        menu.AddItem(WMENU_CHOICE8,"Unlock Warden");
    }
    if(CheckCommandAccess(client,"sm_awmute",2))
    {
        menu.AddItem(WMENU_CHOICE9,"Warden Mute");
    }
    menu.DisplayAt(client,selection,MENU_TIME_FOREVER);
}


public int LRMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch(action)
    {
        case MenuAction_Start:
        {
            //do nothing for now
        }
        case MenuAction_Display:
        {
            char buffer[255];
            Format(buffer, sizeof(buffer), "%T", "LRMenuTitle", param1);
            Panel panel = view_as<Panel>(param2);
            panel.SetTitle(buffer);
        }
        case MenuAction_Select:
        {
            //sanity check just in case someone somehow gets the lr menu opened when not lr winner
            if (param1 != LRConfig.LRWinner)
            {
                delete menu;
                return 0;
            }

            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            if (StrEqual(info, LRCHOICE1))
            {
                ChooseLR(param1,LR_FreedaySelf);
            }
            else if (StrEqual(info, LRCHOICE2))
            {
                ChooseLR(param1,LR_FreedayOthers);
            }
            else if (StrEqual(info, LRCHOICE3))
            {
                ChooseLR(param1,LR_FreedayAll);
            }
            else if (StrEqual(info, LRCHOICE4))
            {
                ChooseLR(param1,LR_Warday);
            }
            else if (StrEqual(info, LRCHOICE5))
            {
                ChooseLR(param1,LR_Custom);
            }
            else
            {
                ChooseMapLR(param1,info);
            }
        }
        case MenuAction_Cancel:
        {
            CPrintToChatAll("%T %T","PluginTag","LRMenuClose")
            //do nothing
        }
        case MenuAction_End:
        {
            LRConfig.menu = null;
            delete menu;
        }
        case MenuAction_DrawItem:
        {
            //do nothing
        }
        case MenuAction_DisplayItem:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            char display[64];
            if (StrEqual(info,LRCHOICE1))
            {
                Format(display,sizeof(display),"%T","LRMenuOptionFreeday",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,LRCHOICE2))
            {
                Format(display,sizeof(display),"%T","LRMenuOptionFreedayOthers",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,LRCHOICE3))
            {
                Format(display,sizeof(display),"%T","LRMenuOptionFreedayAll",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,LRCHOICE4))
            {
                Format(display,sizeof(display),"%T","LRMenuOptionWarday",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,LRCHOICE5))
            {
                Format(display,sizeof(display),"%T","LRMenuOptionCustom",param1);
                return RedrawMenuItem(display);
            }
        }
    }
    return 0;
}

//abstration done so this can be called from elsewhere
public void CreateLRMenu(int client)
{
    Menu menu = new Menu(LRMenuHandler, MENU_ACTIONS_ALL);
    LRConfig.menu = menu;
    menu.SetTitle("%T","LRMenuTitle",client);
    menu.AddItem(LRCHOICE1,"Freeday for yourself");
    menu.AddItem(LRCHOICE2,"Freeday for others");
    menu.AddItem(LRCHOICE3,"Freeday for all");
    menu.AddItem(LRCHOICE4,"Warday");
    menu.AddItem(LRCHOICE5,"Custom");
    if (LRConfig.maplrcount > 0)
    {
        for(int i;i<LRConfig.maplrcount;i++)
        {
            char key[4];
            FormatEx(key, sizeof(key), "%d", i);
            menu.AddItem(key,g_MapLRNames[i])
        }
    }
    menu.ExitButton = false;
    menu.Display(client,MENU_TIME_FOREVER);
}

public int HelpSubMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch (action)
    {
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_Cancel:
        {
            if (param2 == MenuCancel_ExitBack)
            {
                CreateHelpMenu(param1)
            }
        }
        case MenuAction_Select:
        {
            //this menu has nothing selectable
            //nothing to do here
        }
        case MenuAction_DrawItem:
        {
            return ITEMDRAW_DISABLED;
        }
        case MenuAction_DisplayItem:
        {
            char info[64];
            menu.GetItem(param2, info, sizeof(info));
            char display[64];
            Format(display,sizeof(display),"%T",info,param1);
            return RedrawMenuItem(display);
        }
    }
    return 0;
}

public void CreateHelpSubMenu(int client,char[] cmdname)
{
    Menu menu = new Menu(HelpSubMenuHandler,MENU_ACTIONS_ALL);
    menu.SetTitle("%T","HelpMenuTitle",client);
    char helpcmd[64];
    Format(helpcmd,sizeof(helpcmd),"%sCmd",cmdname);
    char helphelp[64];
    Format(helphelp,sizeof(helphelp),"%sHelp",cmdname);
    menu.AddItem(helpcmd,helpcmd);
    menu.AddItem(helphelp,helphelp);
    menu.ExitBackButton = true;
    menu.Display(client, MENU_TIME_FOREVER);
}

public int HelpMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch (action)
    {
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_Cancel:
        {
            //pass
        }
        case MenuAction_Select:
        {
            char info[64];
            menu.GetItem(param2, info, sizeof(info));
            CreateHelpSubMenu(param1,info)
        }
        case MenuAction_DrawItem:
        {
            //pass
        }
        case MenuAction_DisplayItem:
        {
            char info[64];
            menu.GetItem(param2, info, sizeof(info));
            char temp[64];
            Format(temp,sizeof(temp),"%sCmd",info)
            char display[64];
            Format(display,sizeof(display),"%T",temp,param1);
            return RedrawMenuItem(display);
        }
    }
    return 0;
}

//why is this hardcoded, you may ask?
//well, it's the easiest way to allow conditional menu values based on player/config state
public void CreateHelpMenu(int client)
{
    Menu menu = new Menu(HelpMenuHandler,MENU_ACTIONS_ALL);
    menu.SetTitle("%T","HelpMenuTitle",client);
    menu.AddItem("Help","Help");
    menu.AddItem("Warden","Warden");
    menu.AddItem("FF","FF");
    menu.AddItem("Fire","Fire");
    menu.AddItem("LR","LR");
    menu.AddItem("Surrender","Surrender");
    if (client == JBFS.Warden)
    {
        menu.AddItem("WardenMenu","WardenMenu");
        menu.AddItem("Unwarden","Unwarden");
        menu.AddItem("OpenCells","OpenCells");
        menu.AddItem("CloseCells","CloseCells");
        menu.AddItem("WardenFF","WardenFF");
        menu.AddItem("WardenCol","WardenCol");
        menu.AddItem("Marker","Marker");
        menu.AddItem("GiveLR","GiveLR");
        menu.AddItem("WardenMute","WardenMute");
    }
    menu.Display(client, MENU_TIME_FOREVER);
}

public int ClientMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch (action)
    {
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_Cancel:
        {
            g_ClientMenu[param1] = null;
        }
        case MenuAction_Select:
        {
            char target[32];
            menu.GetItem(param2,target,sizeof(target));

            Call_StartFunction(null, g_RunFn[param1]);
            Call_PushCell(param1);
            Call_PushCell(GetClientOfUserId(StringToInt(target)));
            bool result;
            Call_Finish(result);
            //if the runfn returns true, redraw the menu
            //else, do nothing
            if(result)
            {
                CreateClientSelectMenu(param1,g_filterFn[param1],g_RunFn[param1]);
            }
        }
    }

    return 0;
}

public void CreateClientSelectMenu(int client, ClientFilterFunc filterFn, ClientRunFunc runFn)
{
    g_RunFn[client] = runFn;
    g_filterFn[client] = filterFn;
    Menu menu = new Menu(ClientMenuHandler);
    g_ClientMenu[client] = menu;
    int clients;
    for (int i = 1; i <= MaxClients; i++)
    {
        if (!IsClientInGame(i)) continue;

        // Check whether we should add this client to the menu
        Call_StartFunction(null, filterFn);
        Call_PushCell(i);
        bool result;
        Call_Finish(result);

        if (!result) continue;

        char key[12], name[MAX_NAME_LENGTH];
        FormatEx(key, sizeof(key), "%d", GetClientUserId(i));
        GetClientName(i, name, sizeof(name));
        menu.AddItem(key, name);
        clients++
    }
    if (clients==0)
    {
        delete menu;
        g_ClientMenu[client] = null;
        return;
    }
    menu.Display(client, MENU_TIME_FOREVER);
}
public Action Command_GiveLastRequest(int client, int args)
{
    if(client!=JBFS.Warden)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
        return Plugin_Handled;
    }
    if (!cvarJBFS[LastRequest].BoolValue)
    {
        CPrintToChat(client,"%t %t","PluginTag","LastRequestsDisabled");
        return Plugin_Handled;
    }
    if(client) CreateClientSelectMenu(client, ClientEligibleForLR, WardenGiveLR);
    return Plugin_Handled;
}

public bool WardenGiveLR(int warden, int target)
{
    return SetLRWinner(target,warden,false)
}

public Action Command_Admin_ForceLastRequest(int client, int args)
{
    if (!cvarJBFS[LastRequest].BoolValue)
    {
        CPrintToChat(client,"%t %t","PluginTag","LastRequestsDisabled");
        return Plugin_Handled;
    }
    if (!JBFS.RoundActive)
    {
        CPrintToChat(client,"%t %t","PluginTag","CommandWarnRound");
        return Plugin_Handled;
    }
    if (client) CreateClientSelectMenu(client, ClientEligibleForLR, AdminGiveLR);
    return Plugin_Handled;
}

public bool AdminGiveLR(int admin, int target)
{
    return SetLRWinner(target,admin,true,admin)
}

public void OnAdminMenuReady(Handle aTopMenu)
{
    TopMenu topmenu = TopMenu.FromHandle(aTopMenu);

    if (obj_jbfscommands == INVALID_TOPMENUOBJECT)
    {
        OnAdminMenuCreated(topmenu);
    }

    if (topmenu == hAdminMenu)
    {
        return;
    }

    hAdminMenu = topmenu;
}

public void OnAdminMenuCreated(Handle aTopMenu)
{
    TopMenu topmenu = TopMenu.FromHandle(aTopMenu);

    if (topmenu == hAdminMenu && obj_jbfscommands != INVALID_TOPMENUOBJECT)
    {
        return;
    }

    obj_jbfscommands = AddToTopMenu(
        topmenu,
        "JBFS Commands",
        TopMenuObject_Category,
        CategoryHandler,
        INVALID_TOPMENUOBJECT
    );

    AddToTopMenu(topmenu,
        "sm_forcewarden",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_forcewarden",
        cvarJBFS_CMD[ACMD_ForceWarden].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_forceunwarden",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_forceunwarden",
        cvarJBFS_CMD[ACMD_ForceWarden].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_lockwarden",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_lockwarden",
        cvarJBFS_CMD[ACMD_LockWarden].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_unlockwarden",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_unlockwarden",
        cvarJBFS_CMD[ACMD_LockWarden].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_forceopencells",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_forceopencells",
        cvarJBFS_CMD[ACMD_Cells].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_forceclosecells",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_forceclosecells",
        cvarJBFS_CMD[ACMD_Cells].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_aff",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_aff",
        cvarJBFS_CMD[ACMD_FF].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_acc",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_acc",
        cvarJBFS_CMD[ACMD_CC].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_forcelr",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_forcelr",
        cvarJBFS_CMD[ACMD_ForceLR].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_freeday",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_freeday",
        cvarJBFS_CMD[ACMD_ForceFreeday].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_awmute",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_awmute",
        cvarJBFS_CMD[ACMD_WardenMute].IntValue
    );
    AddToTopMenu(topmenu,
        "sm_jailtime",
        TopMenuObject_Item,ItemHandler,obj_jbfscommands,
        "sm_jailtime",
        cvarJBFS_CMD[ACMD_JailTime].IntValue
    );
}

public void CategoryHandler(
    TopMenu topmenu, 
    TopMenuAction action,
    TopMenuObject object_id,
    int param,
    char[] buffer,
    int maxlength)
{
    switch (action)
    {
        case TopMenuAction_DisplayTitle:
        {
            strcopy(buffer, maxlength, "JBFS Commands:");
        }
        case TopMenuAction_DisplayOption:
        {
            strcopy(buffer, maxlength, "JBFS Commands");
        }
    }
}

public void ItemHandler(
    TopMenu topmenu,
    TopMenuAction action,
    TopMenuObject object_id,
    int param,
    char[] buffer,
    int maxlength)
{
    char command[64];
    topmenu.GetObjName(object_id,command,sizeof(command))
    switch (action)
    {
        case TopMenuAction_DisplayOption:
        {
            if (StrEqual(command,"sm_forcewarden"))
            {
                strcopy(buffer,maxlength, "Force Warden");
            }
            else if (StrEqual(command,"sm_forceunwarden"))
            {
                strcopy(buffer,maxlength, "Force Unwarden");
            }
            else if (StrEqual(command,"sm_lockwarden"))
            {
                strcopy(buffer,maxlength, "Lock Warden");
            }
            else if (StrEqual(command,"sm_unlockwarden"))
            {
                strcopy(buffer,maxlength, "Unlock Warden");
            }
            else if (StrEqual(command,"sm_forceopencells"))
            {
                strcopy(buffer,maxlength, "Open Cells");
            }
            else if (StrEqual(command,"sm_forceclosecells"))
            {
                strcopy(buffer,maxlength, "Close Cells");
            }
            else if (StrEqual(command,"sm_aff"))
            {
                strcopy(buffer,maxlength, "Toggle Friendly Fire");
            }
            else if (StrEqual(command,"sm_acc"))
            {
                strcopy(buffer,maxlength, "Toggle Collisions");
            }
            else if (StrEqual(command,"sm_forcelr"))
            {
                strcopy(buffer,maxlength, "Force LR");
            }
            else if (StrEqual(command,"sm_freeday"))
            {
                strcopy(buffer,maxlength, "Force Freeday");
            }
            else if (StrEqual(command,"sm_awmute"))
            {
                strcopy(buffer,maxlength, "Toggle Warden Mute");
            }
            else if (StrEqual(command,"sm_jailtime"))
            {
                strcopy(buffer,maxlength, "Change Round Time");
            }
        }
        case TopMenuAction_SelectOption:
        {
            if (StrEqual(command,"sm_forcewarden"))
            {
                Command_Admin_ForceWarden(param,0);
            }
            else if (StrEqual(command,"sm_forceunwarden"))
            {
                Command_Admin_ForceUnWarden(param,0);
            }
            else if (StrEqual(command,"sm_lockwarden"))
            {
                Command_Admin_LockWarden(param,0);
            }
            else if (StrEqual(command,"sm_unlockwarden"))
            {
                Command_Admin_UnlockWarden(param,0);
            }
            else if (StrEqual(command,"sm_forceopencells"))
            {
                Command_Admin_OpenCells(param,0);
            }
            else if (StrEqual(command,"sm_forceclosecells"))
            {
                Command_Admin_CloseCells(param,0);
            }
            else if (StrEqual(command,"sm_aff"))
            {
                Command_Admin_ToggleFriendlyFire(param,0);
            }
            else if (StrEqual(command,"sm_acc"))
            {
                Command_Admin_ToggleCollisions(param,0);
            }
            else if (StrEqual(command,"sm_forcelr"))
            {
                Command_Admin_ForceLastRequest(param,0);
            }
            else if (StrEqual(command,"sm_freeday"))
            {
                Command_Admin_ForceFreeday(param,0);
            }
            else if (StrEqual(command,"sm_awmute"))
            {
                Command_Admin_WardenMute(param,0);
            }
            else if (StrEqual(command,"sm_jailtime"))
            {
                Command_Admin_JailTime(param,0);
            }
        }
    }
}