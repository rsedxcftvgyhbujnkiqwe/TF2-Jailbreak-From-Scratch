#if defined _jailbreakfromscratch_included
    #endinput
#endif
#define _jailbreakfromscratch_included

/**
 * Add a player to the guardban list
 * This does not modify any databases and is purely for external plugin integration
 * @param client        Client to be Guardbanned
 *
 * @return              true if guard ban was added successfully,
 *                      false if player already guardbanned
 */
native bool JBFS_AddGuardBan(int client);

/**
 * Remove a player from the guardban list
 * This does not modify any databases and is purely for external plugin integration
 * The guardban variable for a player is automatically set to false upon disconnect
 * @param client        Client to be Unguardbanned
 *
 * @return              true if guard ban was removed successfully,
 *                      false if player wasn't guardbanned
 */
native bool JBFS_RemoveGuardBan(int client);

/**
 * Return whether a player is currently guard banned or not
 * @param client		Client to check
 *
 * @return				true if guard banned, false if not
 */
native bool JBFS_IsGuardBanned(int client);

public SharedPlugin __pl_jailbreakfromscratch = {
	name = "JailbreakFromScratch",
	file = "JailbreakFromScratch.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_PLUGIN
public void __pl_jailbreakfromscratch_SetNTVOptional()
{
	MarkNativeAsOptional("JBFS_AddGuardBan");
	MarkNativeAsOptional("JBFS_RemoveGuardBan");
	MarkNativeAsOptional("JBFS_IsGuardBanned");
}
#endif