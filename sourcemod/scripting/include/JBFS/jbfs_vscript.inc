#if defined _vscript_included
VScriptFunction g_GetWarden;

stock void CreateVScriptFunctions()
{
    //GetWarden function
    g_GetWarden = VScript_CreateGlobalFunction("GetWarden");
    g_GetWarden.Return = FIELD_INTEGER;
    g_GetWarden.SetFunctionEmpty();
}

stock void RegisterVScriptFunctions()
{
    RunScript("::jbplugin<-\"JBFS\"");
    //GetWarden function
    g_GetWarden.Register();
    g_GetWarden.CreateDetour().Enable(Hook_Pre,Detour_GetWarden)
}

stock MRESReturn Detour_GetWarden(DHookReturn hReturn)
{
    if (vscript)
    {
        hReturn.Value = GetClientUserId(JBFS.Warden);
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

stock void JBGameEvent(int eventType)
{
    if(vscript)
    {
#if defined _vscript_included
        char script[255];
        switch (eventType)
        {
            case JBEvent_Warday:
            {
                script = "FireGameEvent(\"JB_OnWarday\",{})";
            }
            case JBEvent_SetWarden:
            {
                int clientid;
                if (JBFS.Warden > 0) clientid = GetClientUserId(JBFS.Warden);
                else clientid = 0;
                Format(script,sizeof(script),"FireGameEvent(\"JB_OnSetWarden\",{warden=%d})",clientid);
            }
            case JBEvent_RemoveWarden:
            {
                int clientid;
                if (JBFS.Warden > 0) clientid = GetClientUserId(JBFS.Warden);
                else clientid = 0;
                Format(script,sizeof(script),"FireGameEvent(\"JB_OnRemoveWarden\",{warden=%d})",clientid);
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