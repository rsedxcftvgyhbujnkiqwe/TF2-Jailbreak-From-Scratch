public void LoadConfigs()
{
    LoadMapConfig();
}

public void LoadMapConfig()
{
    char mapcfg[PLATFORM_MAX_PATH] = "configs/jbfs/maps.cfg";
    KeyValues kv = new KeyValues("Maps");
    char path[PLATFORM_MAX_PATH];
    if(!kv.ImportFromFile(mapcfg))
    {
        BuildPath(Path_SM,path,sizeof(path),mapcfg);
    }
    char map[PLATFORM_MAX_PATH];
    GetCurrentMap(map,sizeof(map));

    if(kv.JumpToKey(map))
    {
        //configs found, load existing
        char cellname[PLATFORM_MAX_PATH];
        kv.GetString("Cells",cellname,sizeof(cellname));
        if (StrEqual(MapConfig.cells,"\0"))
        {
            MapConfig.cells = cellname;
            MapConfig.hascells = true;
        }
        if (kv.JumpToKey("LR_Freeday_Spawn"))
        {
            MapConfig.fspawn[0] = kv.GetFloat("CoordX");
            MapConfig.fspawn[1] = kv.GetFloat("CoordY");
            MapConfig.fspawn[2] = kv.GetFloat("CoordZ");
            if(kv.GetNum("Enabled"))
                MapConfig.hasfspawn = true;
            kv.GoBack()
        }
        if (kv.JumpToKey("LR_Red_Spawn"))
        {

            MapConfig.rspawn[0] = kv.GetFloat("CoordX");
            MapConfig.rspawn[1] = kv.GetFloat("CoordY");
            MapConfig.rspawn[2] = kv.GetFloat("CoordZ");
            if(kv.GetNum("Enabled"))
                MapConfig.hasrspawn = true;
            kv.GoBack()
        }
        if (kv.JumpToKey("LR_Blue_Spawn"))
        {
            MapConfig.bspawn[0] = kv.GetFloat("CoordX");
            MapConfig.bspawn[1] = kv.GetFloat("CoordY");
            MapConfig.bspawn[2] = kv.GetFloat("CoordZ");
            if(kv.GetNum("Enabled"))
                MapConfig.hasbspawn = true;
            kv.GoBack()
        }
        delete kv;
    }
    else
    {
        /*
        map config not set! autogenerate one
        cell doors are assumed to be an entity named cell_door
        this can be either:
        -func_door
        -func_door_rotating
        -logic_relay
        -func_breakable
        cell button is assumed to be:
        func_button named open_cells
        */
        int cells = FindEntityByName(-1,"cell_door");
        if (cells > 0)
        {
            MapConfig.cells = "cell_door";
            MapConfig.hascells = true;
        }
        /*
        this section is to allow mappers to set these values on their own using named logic relays
        server config will still override but these work as defaults
        note: can't check if vec exists or not because 0,0,0 could be a correct coord
        */
        int i = -1;
        while((i=FindEntityByClassname(i, "logic_relay"))!=-1)
        {
            char entname[PLATFORM_MAX_PATH];
            GetEntPropString(i,Prop_Data,"m_iName",entname,sizeof(entname));
            if (StrEqual(entname,"lrspawn_freeday"))
            {
                GetEntPropVector(i,Prop_Data,"m_vecAbsOrigin",MapConfig.fspawn);
                MapConfig.hasfspawn = true;
            }
            else if (StrEqual(entname,"lrspawn_red"))
            {
                GetEntPropVector(i,Prop_Data,"m_vecAbsOrigin",MapConfig.rspawn);
                MapConfig.hasrspawn = true;
            }
            else if (StrEqual(entname,"lrspawn_blue"))
            {
                GetEntPropVector(i,Prop_Data,"m_vecAbsOrigin",MapConfig.bspawn);
                MapConfig.hasbspawn = true;
            }
        }

        //write config to file
        kv.JumpToKey(map,true);
        kv.SetString("Cells",MapConfig.cells);
        SetKVCoord(kv,"LR_Freeday_Spawn",MapConfig.fspawn,MapConfig.hasfspawn);
        SetKVCoord(kv,"LR_Red_Spawn",MapConfig.rspawn,MapConfig.hasrspawn);
        SetKVCoord(kv,"LR_Blue_Spawn",MapConfig.bspawn,MapConfig.hasbspawn);
        kv.Rewind();
        kv.ExportToFile(mapcfg);
        delete kv;
    }
    //MapConfig values should be set by now


}

stock SetKVCoord(KeyValues kv, char[] keyname, float vec[3], bool usevec)
{
    kv.JumpToKey(keyname,true);
    float xcoord,ycoord,zcoord;
    int enabled;
    if(usevec)
    {
        xcoord = vec[0];
        ycoord = vec[1];
        zcoord = vec[2];
        enabled = 1;
    }
    else
    {
        xcoord = 0.0;
        ycoord = 0.0;
        zcoord = 0.0;
        enabled = 0;
    }
    kv.SetFloat("CoordX",xcoord);
    kv.SetFloat("CoordY",ycoord);
    kv.SetFloat("CoordZ",zcoord);
    kv.SetNum("Enabled",enabled);
    kv.GoBack();
}