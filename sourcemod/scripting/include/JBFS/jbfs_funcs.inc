public void AutoBalanceTeams()
{
    float flBalanceRatio = cvarJBFS[BalanceRatio].FloatValue;
    int iNumBlue = GetNumPlayers(BLU,true);
    int iNumRed = GetNumPlayers(RED,true);
    int iTotalPlayers = iNumBlue+iNumRed
    //current ratio
    float flRatio = float(iNumBlue)/float(iNumBlue);
    //diff between target ratio and current
    float flDiff = flRatio - flBalanceRatio;
    //if diff <=0, ratio is already achieved
    if (flDiff <= 0)
        return;
    else
    {
        //get number of blues to create final ratio
        int iTargetBlues = iTotalPlayers - RoundToCeil((iTotalPlayers)/(1+flBalanceRatio));
        //number of blues that have to be balanced to achieve the ratio
        int iBalanceNum = iNumBlue-iTargetBlues
        //deal with rounding errors
        if(iBalanceNum==0)
            return;
        else
        {
            for(int i = iBalanceNum;i>0;i--)
            {
                int Player = GetRandomPlayer(BLU,true);
                PrintToServer("%N set to be autobalanced!",Player);
            }
        }
    }
}