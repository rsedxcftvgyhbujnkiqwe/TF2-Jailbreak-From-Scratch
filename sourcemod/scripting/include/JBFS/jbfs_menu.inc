#define WMENU_CHOICE1 "#wmchoice1"
#define WMENU_CHOICE2 "#wmchoice2"

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
    CreateWardenMenu(client,1);
    return Plugin_Handled;
}
//abstration done so this can be called from menu handler
public void CreateWardenMenu(int client,int selection)
{
    Menu menu = new Menu(WardenMenuHandler, MENU_ACTIONS_ALL);
    menu.SetTitle("%T","WardenMenuTitle",LANG_SERVER);
    menu.AddItem(WMENU_CHOICE1,"Open Cells");
    menu.AddItem(WMENU_CHOICE2,"Close Cells");
    menu.DisplayAt(client,1,MENU_TIME_FOREVER);
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
                ModifyCells(true,param1);
            }
            else if (StrEqual(info, WMENU_CHOICE2))
            {
                ModifyCells(false,param1);
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
        }
    }
    return 0;
}

public Action Command_Admin_WardenMenu(int client, int args)
{
    CreateAdminWardenMenu(client,1);
    return Plugin_Handled;
}
public void CreateAdminWardenMenu(int client,int selection)
{
    Menu menu = new Menu(AdminWardenMenuHandler,MENU_ACTIONS_ALL);
    menu.SetTitle("%T","WardenMenuTitle",LANG_SERVER);
    menu.AddItem(WMENU_CHOICE1,"Open Cells");
    menu.AddItem(WMENU_CHOICE2,"Close Cells");
    menu.DisplayAt(client,1,MENU_TIME_FOREVER);
}
