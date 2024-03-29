public void LoadConfigs()
{
    LoadMapConfig();
    LoadLRConfig();
    LoadWeaponConfig();
}

public void LoadMapConfig()
{
    //reset vars
    MapConfig.fspawn = {0.0,0.0,0.0};
    MapConfig.hasfspawn = false;
    MapConfig.rspawn = {0.0,0.0,0.0};
    MapConfig.hasrspawn = false;
    MapConfig.bspawn = {0.0,0.0,0.0};
    MapConfig.hasbspawn = false;
    MapConfig.hascells = false;
    MapConfig.cells = "\0"
    MapConfig.cellclass = "\0"

    char mapcfg[64] = "configs/jbfs/maps.cfg";
    KeyValues kv = new KeyValues("Maps");
    char path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM,path,sizeof(path),mapcfg);

    if(!kv.ImportFromFile(path))  return;

    char map[PLATFORM_MAX_PATH];
    GetCurrentMap(map,sizeof(map));

    if(kv.JumpToKey(map))
    {
        //configs found, load existing
        char cellname[64];
        kv.GetString("Cells",cellname,sizeof(cellname));
        if (!StrEqual(cellname,"\0"))
        {
            MapConfig.cells = cellname;
            int cells = FindEntityByName(-1,"*",MapConfig.cells);
            if (IsValidEntity(cells)){
                MapConfig.hascells = true;
                GetEntityClassname(cells,MapConfig.cellclass,sizeof(MapConfig.cellclass));
            }
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
        PrintToServer("====Config not found for map, generating...====")
        MapConfig.cells = "cell_door";
        int cells = FindEntityByName(-1,"*","cell_door");
        if (IsValidEntity(cells)){
            MapConfig.hascells = true;
            GetEntityClassname(cells,MapConfig.cellclass,sizeof(MapConfig.cellclass));
        }
        /*
        this section is to allow mappers to set these values on their own using named entities
        I would recommend logic_relay because it's not an edict
        server config will still override but these work as defaults
        note: can't check if vec exists or not because 0,0,0 could be a correct coord
        */
        int fspawn = FindEntityByName(-1,"*","lrspawn_freeday");
        if (IsValidEntity(fspawn)){
            GetEntPropVector(fspawn,Prop_Data,"m_vecAbsOrigin",MapConfig.fspawn);
            MapConfig.hasfspawn = true;
        }
        int rspawn = FindEntityByName(-1,"*","lrspawn_red");
        if (IsValidEntity(rspawn)){
            GetEntPropVector(rspawn,Prop_Data,"m_vecAbsOrigin",MapConfig.rspawn);
            MapConfig.hasrspawn = true;
        }
        int bspawn = FindEntityByName(-1,"*","lrspawn_blue");
        if (IsValidEntity(bspawn)){
            GetEntPropVector(bspawn,Prop_Data,"m_vecAbsOrigin",MapConfig.bspawn);
            MapConfig.hasbspawn = true;
        }

        //write config to file
        kv.JumpToKey(map,true);
        kv.SetString("Cells",MapConfig.cells);
        SetKVCoord(kv,"LR_Freeday_Spawn",MapConfig.fspawn,MapConfig.hasfspawn);
        SetKVCoord(kv,"LR_Red_Spawn",MapConfig.rspawn,MapConfig.hasrspawn);
        SetKVCoord(kv,"LR_Blue_Spawn",MapConfig.bspawn,MapConfig.hasbspawn);
        kv.Rewind();
        kv.ExportToFile(path);
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

public void LoadLRConfig()
{
    /*custom map LRs defined by logic relays
    convention: relay must be named "jblr_<Name of LR>"
    name can have spaces and such, for instance
    "jblr_Magic Day"
    this relay will be called on round START, the following round.
    */
    int e = -1;
    char name[64];
    int count;
    while((e = FindEntityByClassname(e,"logic_relay"))!=-1)
    {
        GetEntPropString(e, Prop_Data, "m_iName", name, sizeof(name));
        if (strncmp("jblr_",name,5,false)==0)
        {
            strcopy(g_MapLRNames[count],64,name[5]);
            g_MapLRRelays[count++] = name;
        }
    }
    LRConfig.maplrcount = count;
}

public void LoadWeaponConfig()
{
    char mapcfg[64] = "configs/jbfs/weapons.cfg";
    KeyValues kv = new KeyValues("Weapons");
    char path[PLATFORM_MAX_PATH];
    BuildPath(Path_SM,path,sizeof(path),mapcfg);

    if(!kv.ImportFromFile(path)) return;

    /*
    config structure:
    "Weapons"
    {
        //requires ammo to have
        //guards have by default
        //reds need ammo to acquire
        //for instance: jarate, bonk, etc.
        "Ammo"
        {
            "0"     "123"
        }
        //only available on warday (both teams)
        "Warday"
        {
            "0"     "456"
        }
        //never allowed
        "All"
        {
            "0"     "789"
        }
    }
    */
    //all
    AddWepSec(kv,"Ammo",WeaponConfig.BlockedWeapons_Ammo)
    AddWepSec(kv,"Warday",WeaponConfig.BlockedWeapons_Warday)
    AddWepSec(kv,"All",WeaponConfig.BlockedWeapons_All)
}

stock void AddWepSec(KeyValues kv, char[] sectionname,any sectionarray[128])
{
    kv.Rewind()
    kv.JumpToKey(sectionname,true)
    if(!kv.GotoFirstSubKey(false)){
        kv.JumpToKey("0",true);
        kv.SetNum(NULL_STRING,-1);
    }
    int count;
    SetArray(sectionarray,sizeof(sectionarray),-1);
    do
    {
        int wep = kv.GetNum(NULL_STRING);
        sectionarray[count++] = wep;
    }
    while(kv.GotoNextKey(false))
}