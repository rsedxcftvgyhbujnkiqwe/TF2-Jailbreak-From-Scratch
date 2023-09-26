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
    RunScript("::jbplugin<-JBFS");
    //GetWarden function
    g_GetWarden.Register();
    g_GetWarden.CreateDetour().Enable(Hook_Pre,Detour_GetWarden)
}

stock MRESReturn Detour_GetWarden(DHookReturn hReturn)
{
    if (vscript)
    {
        hReturn.SetInt(GetClientUserId(JBFS.Warden));
        return MRES_Supercede;
    }
}
#endif

stock void JBGameEvent(int eventType)
{
    if(vscript)
    {
#if defined _vscript_included
        HSCRIPT script;
        switch (eventType)
        {
            case JBEvent_Warday:
            {
                script = VScript_CompileScript("FireGameEvent(\"JB_OnWarday\",{})");
            }
            case JBEvent_SetWarden:
            {
                char scriptstring[64];
                int clientid;
                if (JBFS.Warden > 0) clientid = GetClientUserId(JBFS.Warden);
                else clientid = 0;
                Format(scriptstring,sizeof(scriptstring,"FireGameEvent(\"JB_OnSetWarden\",{warden=%d})"),clientid);
                script = VScript_CompileScript(scriptstring);
            }
            case JBEvent_RemoveWarden:
            {
                char scriptstring[64];
                int clientid;
                if (JBFS.Warden > 0) clientid = GetClientUserId(JBFS.Warden);
                else clientid = 0;
                Format(scriptstring,sizeof(scriptstring,"FireGameEvent(\"JB_OnRemoveWarden\",{warden=%d})"),clientid);
                script = VScript_CompileScript(scriptstring);
            }
            default
            {
                script.ReleaseScript();
                return;
            }
        }
        VScriptExecute execute = new VScriptExecute(script);
        execute.Execute();
        delete execute;
        script.ReleaseScript();
#endif
    }
}