#define CHOICE1 "#choice1"
#define CHOICE2 "#choice2"
#define CHOICE3 "#choice3"
 
public int MenuHandler1(Menu menu, MenuAction action, int param1, int param2)
{
  switch(action)
  {
    case MenuAction_Start:
    {
      PrintToServer("Displaying menu");
    }
 
    case MenuAction_Display:
    {
      char buffer[255];
      Format(buffer, sizeof(buffer), "%T", "Vote Nextmap", param1);
 
      Panel panel = view_as<Panel>(param2);
      panel.SetTitle(buffer);
      PrintToServer("Client %d was sent menu with panel %x", param1, param2);
    }
 
    case MenuAction_Select:
    {
      char info[32];
      menu.GetItem(param2, info, sizeof(info));
      if (StrEqual(info, CHOICE3))
      {
        PrintToServer("Client %d somehow selected %s despite it being disabled", param1, info);
      }
      else
      {
        PrintToServer("Client %d selected %s", param1, info);
      }
    }
 
    case MenuAction_Cancel:
    {
      PrintToServer("Client %d's menu was cancelled for reason %d", param1, param2);
    }
 
    case MenuAction_End:
    {
      delete menu;
    }
 
    case MenuAction_DrawItem:
    {
      int style;
      char info[32];
      menu.GetItem(param2, info, sizeof(info), style);
 
      if (StrEqual(info, CHOICE3))
      {
        return ITEMDRAW_DISABLED;
      }
      else
      {
        return style;
      }
    }
 
    case MenuAction_DisplayItem:
    {
      char info[32];
      menu.GetItem(param2, info, sizeof(info));
 
      char display[64];
 
      if (StrEqual(info, CHOICE3))
      {
        Format(display, sizeof(display), "%T", "Choice 3", param1);
        return RedrawMenuItem(display);
      }
    }
  }
 
  return 0;
}
 
public Action Menu_Test1(int client, int args)
{
  Menu menu = new Menu(MenuHandler1, MENU_ACTIONS_ALL);
  menu.SetTitle("%T", "Menu Title", LANG_SERVER);
  menu.AddItem(CHOICE1, "Choice 1");
  menu.AddItem(CHOICE2, "Choice 2");
  menu.AddItem(CHOICE3, "Choice 3");
  menu.ExitButton = false;
  menu.Display(client, 20);
 
  return Plugin_Handled;
}