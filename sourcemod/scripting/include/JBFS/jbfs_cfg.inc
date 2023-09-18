public void LoadConfigs()
{
    LoadMapConfig();
}

public void LoadMapConfig()
{
    char mapcfg[32] = "configs/jbfs/maps.cfg";
    KeyValues kv = new KeyValues("Maps");
    char path[PLATFORM_MAX_PATH];
    if(!kv.ImportFromFile(path))
    {
        BuildPath(Path_SM,path,sizeof(path),mapcfg);
    }
    char map[128];
    GetCurrentMap(map,sizeof(map));

    if(kv.JumpToKey(map))
    {
        //configs found, load existing
        char cellname[128];
        kv.GetString("Cells",cellname,sizeof(cellname));
        if (MapConfig.cells != "\0")
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
        cell doors are assumed to be:
        func_door named cell_door
        cell button is assumed to be:
        func_button named open_cells
        */
        int i;
        while((i=FindEntityByClassname(i, "func_door"))!=-1)
        {
            char entname[32];
            if((entname=GetEntPropString(i,Prop_Data,"m_iName",entname,sizeof(entname)))=="cell_door")
            {
                MapConfig.cells = entname;
                MapConfig.hascells = true;
                break;
            }
        }
        /*
        this section is to allow mappers to set these values on their own using named logic relays
        server config will still override but these work as defaults
        note: can't check if vec exists or not because 0,0,0 could be a correct coord
        */
        int i;
        while((i=FindEntityByClassname(i, "logic_relay"))!=-1)
        {
            char entname[32];
            entname=GetEntPropString(i,Prop_Data,"m_iName",entname,sizeof(entname));
            switch(entname)
            {
                case "lrspawn_freeday":
                {
                    GetEntPropVector(i,Prop_Data,"m_vecAbsOrigin",MapConfig.fspawn);
                    MapConfig.hasfspawn = true;
                }
                case "lrspawn_red":
                {
                    GetEntPropVector(i,Prop_Data,"m_vecAbsOrigin",MapConfig.rspawn);
                    MapConfig.hasrspawn = true;
                }
                case "lrspawn_blue":
                {
                    GetEntPropVector(i,Prop_Data,"m_vecAbsOrigin",MapConfig.bspawn);
                    MapConfig.hasbspawn = true;
                }
            }
        }

        //write config to file
        kv.JumpToKey(map,true);
        kv.SetString("Cells",cells);
        SetKVCoord(kv,"LR_Freeday_Spawn",MapConfig.fspawn,MapConfig.hasfspawn);
        SetKVCoord(kv,"LR_Red_Spawn",MapConfig.rspawn,MapConfig.hasrspawn);
        SetKVCoord(kv,"LR_Blue_Spawn",MapConfig.bspawn,MapConfig.hasbspawn);
        kv.Rewind();
        kv.ExportToFile(mapcfg);
        delete kv;
    }
    //MapConfig values should be set by now


}

stock SetKVCoord(KeyValues kv, char keyname[32], float vec[3], bool usevec)
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
        xcoord = 0;
        ycoord = 0;
        zcoord = 0;
        enabled = 0;
    }
    kv.SetFloat("CoordX",xcoord);
    kv.SetFloat("CoordY",ycoord);
    kv.SetFloat("CoordZ",zcoord);
    kv.SetNum("Enabled",enabled);
    kv.GoBack();
}