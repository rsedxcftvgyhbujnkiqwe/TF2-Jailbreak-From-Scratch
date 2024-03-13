## JBFS v1.2.1
### Changes
- Round timer is no longer hudtext, instead using the built in arena timer
### Bug Fixes
- Fixed issue where admin command re-registered every map change, causing multiple firings
## JBFS Gangs v1.0
### Features
- Players can create gangs, which are stored in an SQL database in order to function across servers/sessions.
- Players can invite other players to their gangs
- Players can use gang chat to communicate secretly with other gang members
- Gangs can purchase gang-wide buffs, using points gained end of round
- Gangs can set a tag, which if enabled will show up in chat for other players to see
### Commands
See ingame gang help/wiki for more aliases for each command
- `sm_ganghelp` - Lists all gang related commands the user has access to
- `sm_gangcreate` - Allows users to create gangs
- `sm_gangjoin` - Allows users to join gangs they've been invited to
- `sm_gangmenu` - Opens gang menu, with gang related commands
- `sm_gangleave` - Allows users to leave their current gangs
- `sm_gangchat` - Allows users to toggle gang chat. All messages will be send to gang members only
- `sm_ganglist` - Lists all online+offline members of user's gang
- `sm_gangpoints` - Allows users to see their gang's current point total
- `sm_ganginvite` - Allows users to invite other users to their gang
- `sm_gangname` - Allows uers to change their gang's name
- `sm_gangtag` - Allows users to set a gang tag
- `sm_gangpromote` - Allows users to promote other users within their gang
- `sm_gangdemote` - Allows users to demote other users within their gang
- `sm_gangkick` - Allows users to kick other players from their gang
### ConVars
- `sm_jbfsg_database` - Name of SQL database for gangs. REQUIRED for plugin to function.
- `sm_jbfsg_announcegangcreate`, `sm_jbfsg_announcegangname` - Configures whether certain gang changes are announced to all players
- `sm_jbfsg_gangchat` - Enables the Gang Chat feature. On by default, highly recommended!
- `sm_jbfsg_maxgangsize` - Controls the maximum number of players that can be in a gang
- `sm_jbfsg_gangtags` - Enables gang tags showing up in chat. On by default
### Natives/VScript
None currently planned
## JBFS Timeout v1.1
### Bug Fixes
- Added string sanitization for player input
- Fixed localized guardbans not being removed on disconnect
## JBFS v1.2
### Changes
- On freedays, cell doors are now killed 3 seconds after round start instead of 5 seconds.
- Warden menu now displays Last Request button, when pressed shows usage
- LRMenu now prints to all when it is closed
- All hud text (round timer, aim name, search progress) now use HudSynchronizers, which automatically choose channels. 
### New Features
- Added hook for dropped weapons, so internal ammo management is more consistent. Picking up dropped weapons now removes freeday, does not return ammo weapons.
- Added AimName system. Warden/Guards (configurable) are able to see the names of prisoners by looking at them.
- Added Search system. Guards are able to search prisoners for weapons by standing near them and holding reload
- Added Gang plugin (see JBFS Gangs v1.0) 
- Added partial chinese translations (thanks to q1085909155)
### Commands
- Added `sm_surrender` command, allows prisoners to surrender ammo (with cooldown)
### ConVars
- Added `sm_jbfs_allowsurrender` ConVar, toggles surrender system
- Added `sm_jbfs_aimnames`, `sm_jbfs_aimtime`, `sm_jbfs_aimdistance` ConVars, which configure the Aim Name system
- Added `sm_jbfs_weaponsearch`, `sm_jbfs_weaponsearchtime` ConVars, which configure the weapon search system
- Added `sm_jbfs_wardenlocktime` ConVar, which controls round time clamping after warden is locked
- Removed `sm_jbfs_textchannel` ConVar, made obsolete by HudSynchronizers
### VScript
- Added global function `SetRebel`. Calling it and passing a userid of a client will cause them to lose freeday. Not the same as picking up ammo, see `TakenAmmo`. If `TakenAmmo` is called, this is redundant.
- Added global function `IsFreeday`. Calling it with a client userid will return true/false depending if that player is currently a freeday.
- Added global function `TakenAmmo`. Calling it with a client userid will be considered by the plugin as picking up ammo, enabling any ammo related features on top of setting them as a rebel. If this is called, `SetRebel` is redundant. Useful if you manage ammo in ways not detected by the plugin.
- Added `JB_OnSurrender` script hook. Called when a prisoner issues the `sm_surrender` command. Useful if you manage ammo in ways not detected by the plugin.
### Natives
- Added `JBFS_GetLrWinner`, returns the client index of the LR winner
- Added `JBFS_IsWarday`, returns whether warday is active or not
- Added `JBFS_IsFreeday`, returns whether client is a freeday or not
### Bug Fixes
- Fixed collisions not being reset on round start
- Removed exit button from LR menu
- Fixed bug where ammo weapons were removed on warday
- Fixed multiple cell doors with the same name not being considered
- Fixed many admin commands from being able to be run pre/post round when they shouldn't be
- Unwarden messages now only fire if not applied to console
- Weapons configured as Ammo Weps are no longer removed during warday

## JBFS v1.1
- Added LastRequest ConVar, which can be used to disable the LastRequest system entirely
- Added ConVars for Warden commands, so that they can be disabled. If disabled, a command will not show up in the warden menu. This will not disable the admin command equivalent.
- Setting marker time to 0 no longer disables the marker system, instead disabling the command will now disable markers
- Fixed timer issue for lr command on round start
- Improved logging and message displays for admin commands
- Added Timeout plugin! A rudamentary teamban system that uses an sql database to store bans. In this plugin, bans are done on a round basis, rather than a "number of minutes" basis. This is partially me pushing my own ideology, but it also makes more sense from a round-based game perspective.
