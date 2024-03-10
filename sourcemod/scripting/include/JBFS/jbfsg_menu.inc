#define GMENUCHOICE_HELP "#gmenuchoicehelp"
#define GMENUCHOICE_NOGANG1 "#gmenuchoicenogang1"
#define GMENUCHOICE_NOGANG2 "#gmenuchoicenogang2"
#define GMENUCHOICE_GANG1 "#gmenuchoicegang1"
#define GMENUCHOICE_GANG2 "#gmenuchoicegang2"
#define GMENUCHOICE_GANG3 "#gmenuchoicegang3"
#define GMENUCHOICE_GANG4 "#gmenuchoicegang4"
#define GMENUCHOICE_GANG5 "#gmenuchoicegang5"
#define GMENUCHOICE_GANG6 "#gmenuchoicegang6"
#define GMENUCHOICE_GANG7 "#gmenuchoicegang7"
#define GMENUCHOICE_GANG8 "#gmenuchoicegang8"
#define GMENUCHOICE_GANG9 "#gmenuchoicegang9"
#define GMENUCHOICE_GANG10 "#gmenuchoicegang10"
#define GSHOP1 "#gshopchoice1"
#define GSHOP2 "#gshopchoice2"
#define GSHOP3 "#gshopchoice3"
#define GSHOP4 "#gshopchoice4"

public int GangMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch(action)
    {
        case MenuAction_Display:
        {
            char buffer[255];
            Format(buffer, sizeof(buffer), "%T", "GangMenuTitle", param1);
            Panel panel = view_as<Panel>(param2);
            panel.SetTitle(buffer);
        }
        case MenuAction_Select:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));

            float gametime = GetGameTime();
                //0.5s cooldown between picks
            if (gametime - g_PlayerData[param1].gmenu_time < 0.5)
            {
                CreateGangMenu(param1, menu.Selection)
                return 0;
            }

            g_PlayerData[param1].gmenu_time = gametime;

            //help
            bool reopen_menu = true;
            if (StrEqual(info,GMENUCHOICE_HELP))
            {
                Command_GangHelp(param1,0);
            }
            //non gang
            else if (StrEqual(info,GMENUCHOICE_NOGANG1))
            {
                Command_CreateGang(param1,0);
            }
            else if (StrEqual(info,GMENUCHOICE_NOGANG2))
            {
                Command_GangJoin(param1,0);
            }
            //just in case!
            if (g_PlayerData[param1].index > 0)
            {
                //gang
                if (StrEqual(info, GMENUCHOICE_GANG1))
                {
                    ListGang(param1);
                }
                else if (StrEqual(info, GMENUCHOICE_GANG2))
                {
                    Command_GangPoints(param1,0);
                }
                else if (StrEqual(info, GMENUCHOICE_GANG3))
                {
                    Command_GangChat(param1,0);
                }
                //officer
                else if (StrEqual(info, GMENUCHOICE_GANG4) && g_PlayerData[param1].rank >= GangRank_Officer)
                {
                    CreateClientSelectMenu(param1,FilterFn_Inviteable,RunFn_InivteToGang,CreateGangMenu);
                    reopen_menu = false;
                }
                else if (StrEqual(info, GMENUCHOICE_GANG5) && g_PlayerData[param1].rank >= GangRank_Officer)
                {
                    CreateShopMenu(param1,0);
                    reopen_menu = false;
                }
                //boss
                else if (StrEqual(info, GMENUCHOICE_GANG6))
                {
                    Command_SetGangName(param1,0);
                }
                else if (StrEqual(info, GMENUCHOICE_GANG7))
                {
                    Command_SetGangTag(param1,0);
                }
                else if (StrEqual(info, GMENUCHOICE_GANG8))
                {
                    Command_GangPromote(param1,0);
                }
                else if (StrEqual(info, GMENUCHOICE_GANG9))
                {
                    Command_GangDemote(param1,0);
                }
                else if (StrEqual(info, GMENUCHOICE_GANG10))
                {
                    Command_GangKick(param1,0);
                }
            }
            if (reopen_menu)
            {
                CreateGangMenu(param1,menu.Selection);
            }
        }
        case MenuAction_Cancel:
        {
            g_GangMenu[param1] = null;
        }
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_DisplayItem:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            char display[64];
            if (StrEqual(info,GMENUCHOICE_HELP))
            {
                Format(display,sizeof(display),"%T","GangMenuOptHelp",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_NOGANG1))
            {
                Format(display,sizeof(display),"%T","GangMenuOptCreateGang",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_NOGANG2))
            {
                Format(display,sizeof(display),"%T","GangMenuOptJoinGang",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG1))
            {
                Format(display,sizeof(display),"%T","GangMenuOptListGang",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG2))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangPoints",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG3))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangChat",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG4))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangInvite",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG5))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangShop",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG6))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangName",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG7))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangTag",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG8))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangPromote",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG9))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangDemote",param1);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GMENUCHOICE_GANG10))
            {
                Format(display,sizeof(display),"%T","GangMenuOptGangKick",param1);
                return RedrawMenuItem(display);
            }
        }
    }
    return 0;
}

//menu can be used if you are in a gang, or not!
public void CreateGangMenu(int client,int selection)
{
    Menu menu = new Menu(GangMenuHandler, MENU_ACTIONS_ALL);
    g_GangMenu[client] = menu;
    menu.SetTitle("%T","GangMenuTitle",client);
    menu.AddItem(GMENUCHOICE_HELP,"Gang Help");
    if (g_PlayerData[client].index == 0)
    {
        menu.AddItem(GMENUCHOICE_NOGANG1,"Create Gang");
        menu.AddItem(GMENUCHOICE_NOGANG2,"Join Gang");
    }
    else
    {
        menu.AddItem(GMENUCHOICE_GANG1,"List Gang");
        menu.AddItem(GMENUCHOICE_GANG2,"Gang Points");
        menu.AddItem(GMENUCHOICE_GANG3,"Gang Chat")
        if (g_PlayerData[client].rank >= GangRank_Officer)
        {
            menu.AddItem(GMENUCHOICE_GANG4,"Gang Invite");
            menu.AddItem(GMENUCHOICE_GANG5,"Gang Shop");
        }
        if (g_PlayerData[client].rank >= GangRank_Boss)
        {
            menu.AddItem(GMENUCHOICE_GANG6,"Gang Name");
            menu.AddItem(GMENUCHOICE_GANG7,"Gang Tag");
            menu.AddItem(GMENUCHOICE_GANG8,"Promote");
            menu.AddItem(GMENUCHOICE_GANG9,"Demote");
            menu.AddItem(GMENUCHOICE_GANG10,"Kick");
        }
    }
    menu.DisplayAt(client,selection,MENU_TIME_FOREVER);
}

public int ShopMenuHandler(Menu menu, MenuAction action, int param1, int param2)
{
    switch(action)
    {
        case MenuAction_Display:
        {
            char buffer[255];
            Format(buffer, sizeof(buffer), "%T", "ShopMenuTitle", param1,g_GangData[g_PlayerData[param1].index].points);
            Panel panel = view_as<Panel>(param2);
            panel.SetTitle(buffer);
        }
        case MenuAction_Select:
        {
            //delete if player not in gang or not of rank
            if (g_PlayerData[param1].index == 0 || g_PlayerData[param1].rank < GangRank_Officer)
            {
                delete menu;
                return 0;
            }
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            if (StrEqual(info,GSHOP1))
            {
                GangShopPurchase(param1,GangShop_Banner);
            }
            else if (StrEqual(info,GSHOP2))
            {
                GangShopPurchase(param1,GangShop_Conch);
            }
            else if (StrEqual(info,GSHOP3))
            {
                GangShopPurchase(param1,GangShop_Backup);
            }
            else if (StrEqual(info,GSHOP4))
            {
                GangShopPurchase(param1,GangShop_Warp);
            }
            CreateShopMenu(param1,0);
        }
        case MenuAction_Cancel:
        {
            g_ShopMenu[param1] = null;
            if (param2 == MenuCancel_ExitBack)
            {
                CreateGangMenu(param1,0);
            }
        }
        case MenuAction_End:
        {
            delete menu;
        }
        case MenuAction_DisplayItem:
        {
            char info[32];
            menu.GetItem(param2, info, sizeof(info));
            char display[64];
            if (StrEqual(info,GSHOP1))
            {
                Format(display,sizeof(display),"%T- %d",g_ShopNames[GangShop_Banner],param1,g_ShopPoints[GangShop_Banner]);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GSHOP2))
            {
                Format(display,sizeof(display),"%T - %d",g_ShopNames[GangShop_Conch],param1,g_ShopPoints[GangShop_Conch]);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GSHOP3))
            {
                Format(display,sizeof(display),"%T- %d",g_ShopNames[GangShop_Backup],param1,g_ShopPoints[GangShop_Backup]);
                return RedrawMenuItem(display);
            }
            else if (StrEqual(info,GSHOP4))
            {
                Format(display,sizeof(display),"%T- %d",g_ShopNames[GangShop_Warp],param1,g_ShopPoints[GangShop_Warp]);
                return RedrawMenuItem(display);
            }
        }
    }
    return 0;
}

//don't create menu if not in gang
public void CreateShopMenu(int client,int selection)
{
    int gang_index = g_PlayerData[client].index;
    Menu menu = new Menu(ShopMenuHandler, MENU_ACTIONS_ALL);
    g_ShopMenu[client] = menu;
    menu.SetTitle("%t","ShopMenuTitle",client);
    if (!(g_GangData[gang_index].shopbought & 1 << GangShop_Banner)){
        menu.AddItem(GSHOP1,"Banner");
    }
    if (!(g_GangData[gang_index].shopbought & 1 << GangShop_Conch)){
        menu.AddItem(GSHOP2,"Conch");
    }
    if (!(g_GangData[gang_index].shopbought & 1 << GangShop_Backup)){
        menu.AddItem(GSHOP3,"Backup");
    }
    if (!(g_GangData[gang_index].shopbought & 1 << GangShop_Warp)){
        menu.AddItem(GSHOP4,"Warp");
    }
    if (menu.ItemCount == 0){
        CreateGangMenu(client,0);
        CPrintToChat(client,"%t %t","PluginGangTag","ShopMenuNoneLeft")
        delete menu;
        g_ShopMenu[client] = null;
        return;
    }
    menu.ExitBackButton = true;
    menu.DisplayAt(client,selection,MENU_TIME_FOREVER);
}


//client select menu

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
                CreateClientSelectMenu(param1,g_filterFn[param1],g_RunFn[param1],g_CallbackFn[param1]);
            }
        }
    }

    return 0;
}

public void CreateClientSelectMenu(int client, ClientFilterFunc filterFn, ClientRunFunc runFn, MenuCallbackFunc callbackFn)
{
    g_RunFn[client] = runFn;
    g_filterFn[client] = filterFn;
    g_CallbackFn[client] = callbackFn;
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
        //callback fn in case of failure
        Call_StartFunction(null,callbackFn)
        Call_PushCell(client);
        Call_PushCell(0);
        Call_Finish();
        return;
    }
    menu.Display(client, MENU_TIME_FOREVER);
}

public bool FilterFn_Inviteable(int target)
{
    if (g_PlayerData[target].index == 0) return true;
    return false;
}

public bool RunFn_InivteToGang(int client, int target)
{
    InviteToGang(client,target,g_GangData[g_PlayerData[client].index].id);
    return true
}