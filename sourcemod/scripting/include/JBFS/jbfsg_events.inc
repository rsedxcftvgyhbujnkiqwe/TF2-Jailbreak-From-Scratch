public void OnClientPutInServer(int client)
{
    int gang_uid = DB_GetPlayerGang(client);
    int rank = GangRank_None;
    int mid = 0;
    if (gang_uid >= 0)
    {
        rank = DB_GetPlayerGangRank(client);
        mid = DB_GetPlayerMID(client,gang_uid);
    }
    SetPlayerGang(client,gang_uid,rank,mid,false);
}

public void OnClientDisconnect(int client)
{
    ResetClientGangData(client);
}