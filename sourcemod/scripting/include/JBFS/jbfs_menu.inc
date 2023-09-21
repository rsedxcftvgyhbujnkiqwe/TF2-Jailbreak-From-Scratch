#define WMENU_CHOICE1 "#wmchoice1"
#define WMENU_CHOICE2 "#wmchoice2"
#define WMENU_CHOICE3 "#wmchoice3"
#define WMENU_CHOICE4 "#wmchoice4"
#define WMENU_CHOICE5 "#wmchoice5"
#define WMENU_CHOICE6 "#wmchoice6"
#define WMENU_CHOICE7 "#wmchoice7"
#define WMENU_CHOICE8 "#wmchoice8"

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
            CreateWardenMenu(param1, menu.Selection)
        }
        case MenuAction_Cancel:
        {
            switch(param2)
            {
                case MenuCancel_Exit,MenuCancel_NoDisplay:
                {
                    CPrintToChat(param1,"%t %t","PluginTag","WardenMenuReOpen");
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
        }
    }
    return 0;
}

public Action Command_WardenMenu(int client, int args)
{
    if(client!=JBFS.Warden)
    {
        CPrintToChat(client,"%t %t","PluginTag","WardenWarnNotWarden");
        return Plugin_Handled;
    }
    CreateWardenMenu(client,0);
    return Plugin_Handled;
}
//abstration done so this can be called from menu handler
public void CreateWardenMenu(int client,int selection)
{
    Menu menu = new Menu(WardenMenuHandler, MENU_ACTIONS_ALL);
    menu.SetTitle("%T","WardenMenuTitle",LANG_SERVER);
    menu.AddItem(WMENU_CHOICE1,"Open Cells");
    menu.AddItem(WMENU_CHOICE2,"Close Cells");
    menu.AddItem(WMENU_CHOICE3,"Toggle Friendly-Fire");
    menu.AddItem(WMENU_CHOICE4,"Toggle Collisions");
    menu.DisplayAt(client,selection,MENU_TIME_FOREVER);
    g_WMenu[client] = menu;
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
            CreateAdminWardenMenu(param1, menu.Selection)
        }
        case MenuAction_Cancel:
        {
            switch(param2)
            {
                case MenuCancel_Exit,MenuCancel_NoDisplay:
                {
                    CPrintToChat(param1,"%t %t","PluginTag","WardenMenuReOpen");
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
    menu.SetTitle("%T","WardenMenuTitle",LANG_SERVER);
    if(CheckCommandAccess(param1,"sm_forceopencells"))
    {
        menu.AddItem(WMENU_CHOICE1,"Open Cells");
        menu.AddItem(WMENU_CHOICE2,"Close Cells");
    }
    if(CheckCommandAccess(param1,"sm_adminff"))
        menu.AddItem(WMENU_CHOICE3,"Toggle Friendly-Fire");
    if(CheckCommandAccess(param1,"sm_admincc"))
        menu.AddItem(WMENU_CHOICE4,"Toggle Collisions");
    if(CheckCommandAccess(param1,"sm_forcewarden"))
    {
        menu.AddItem(WMENU_CHOICE5,"Force Random Warden");
        menu.AddItem(WMENU_CHOICE6,"Force Warden to Retire");
    }
    if(CheckCommandAccess(param1,"sm_lockwarden"))
    {
        menu.AddItem(WMENU_CHOICE7,"Lock Warden");
        menu.AddItem(WMENU_CHOICE8,"Unlock Warden");
    }
    menu.DisplayAt(client,selection,MENU_TIME_FOREVER);
}
