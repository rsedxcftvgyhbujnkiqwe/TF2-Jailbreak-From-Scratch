#if defined _vscript_included
VScriptFunction g_GetWarden;
VScriptFunction g_RemoveFreeday;
VScriptFunction g_IsFreeday;
VScriptFunction g_TakenAmmo;

stock void CreateVScriptFunctions()
{
    //GetWarden function
    g_GetWarden = VScript_CreateGlobalFunction("GetWarden");
    g_GetWarden.Return = FIELD_INTEGER;
    g_GetWarden.SetFunctionEmpty();

    //RemoveFreeday function
    g_RemoveFreeday = VScript_CreateGlobalFunction("RemoveFreeday");
    g_RemoveFreeday.SetParam(1,FIELD_INTEGER);
    g_RemoveFreeday.Return = FIELD_VOID;
    g_RemoveFreeday.SetFunctionEmpty();

    //IsFreeday function
    g_IsFreeday = VScript_CreateGlobalFunction("IsFreeday");
    g_IsFreeday.SetParam(1,FIELD_INTEGER);
    g_IsFreeday.Return = FIELD_BOOLEAN;
    g_IsFreeday.SetFunctionEmpty();

    //TakenAmmo function
    g_TakenAmmo = VScript_CreateGlobalFunction("TakenAmmo");
    g_TakenAmmo.SetParam(1,FIELD_INTEGER);
    g_TakenAmmo.SetParam(2,FIELD_BOOLEAN);
    g_TakenAmmo.Return = FIELD_VOID;
    g_TakenAmmo.SetFunctionEmpty();
}

stock void RegisterVScriptFunctions()
{
    RunScript("::jbplugin<-\"JBFS\"");
    //GetWarden function
    g_GetWarden.Register();
    g_GetWarden.CreateDetour().Enable(Hook_Pre,Detour_GetWarden);
    //RemoveFreeday function
    g_RemoveFreeday.Register();
    g_RemoveFreeday.CreateDetour().Enable(Hook_Pre,Detour_RemoveFreeday);
    //IsFreeday function
    g_IsFreeday.Register();
    g_IsFreeday.CreateDetour().Enable(Hook_Pre,Detour_IsFreeday);
    //TakenAmmo function
    g_TakenAmmo.Register();
    g_TakenAmmo.CreateDetour().Enable(Hook_Pre,Detour_TakenAmmo);
}

stock MRESReturn Detour_GetWarden(DHookReturn hReturn)
{
    if (vscript)
    {
        if (JBFS.Warden > 0) hReturn.Value = GetClientUserId(JBFS.Warden);
        else hReturn.Value = 0;
        return MRES_Supercede;
    }
    return MRES_Ignored;
}

stock MRESReturn Detour_RemoveFreeday(DHookReturn hReturn, DHookParam hParam)
{
    if (vscript)
    {
        int userid = hParam.Get(1);
        if (userid > 0)
        {
            int client = GetClientOfUserId(userid);
            RemoveFreeday(client);
            return MRES_Supercede;
        }
    }
    return MRES_Ignored;
}

stock MRESReturn Detour_IsFreeday(DHookReturn hReturn, DHookParam hParam)
{
    if (vscript)
    {
        int userid = hParam.Get(1);
        if (userid > 0)
        {
            int client = GetClientOfUserId(userid);
            hReturn.Value = LRConfig.FreedaysActive[client];
        }
        else hReturn.Value = false;
        return MRES_Supercede;
    }
    return MRES_Ignored;
}

stock MRESReturn Detour_TakenAmmo(DHookReturn hReturn, DHookParam hParam)
{
    if (vscript)
    {
        int userid = hParam.Get(1);
        if (userid > 0)
        {
            int client = GetClientOfUserId(userid);
            bool return_weapons = hParam.Get(2);
            AmmoTaken(client,return_weapons);
        }
        return MRES_Supercede;
    }
    return MRES_Ignored;
}

stock any RunScript(const char[] sScript, HSCRIPT pScope = HSCRIPT_RootTable)
{
	HSCRIPT pCompile = VScript_CompileScript(sScript);
	VScriptExecute hExecute = new VScriptExecute(pCompile, pScope);
	hExecute.Execute();
	any nReturn = hExecute.ReturnValue;

	delete hExecute;

	pCompile.ReleaseScript();
	return nReturn;
}

#endif

stock void JBGameEvent(int eventType,int client = -1)
{
    if(vscript)
    {
#if defined _vscript_included

        int clientid;
        if (client > 0) clientid = GetClientOfUserId(client);
        else clientid = 0;

        char script[255];
        switch (eventType)
        {
            case JBEvent_Warday:
            {
                script = "FireScriptEvent(\"JB_OnWarday\",{})";
            }
            case JBEvent_SetWarden:
            {
                Format(script,sizeof(script),"FireScriptEvent(\"JB_OnSetWarden\",{warden=%d})",clientid);
            }
            case JBEvent_RemoveWarden:
            {
                Format(script,sizeof(script),"FireScriptEvent(\"JB_OnRemoveWarden\",{warden=%d})",clientid);
            }
            case JBEvent_Surrender:
            {
                Format(script,sizeof(script),"FireScriptEvent(\"JB_OnSurrender\",{userid=%d})",clientid)
            }
            default:
            {
                return;
            }
        }
        RunScript(script);
#endif
    }
}