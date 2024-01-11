public MRESReturn Detour_OnInitPickedUpWeapon_Post(Handle hParams)
{
    OnTakeWeapon( DHookGetParam(hParams,1),DHookGetParam(hParams,2))
}