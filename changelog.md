### JBFS v1.2
- Added Surrender command which allows prisoners to surrender their ammo
- Picking up a dropped weapon will now remove freeday and will set the player as having ammo. Does not return ammo weapons
- Removed exit button from LR menu
- Fixed bug where ammo weapons were removed on warday
- Added chinese translations (thanks to q1085909155)
- Added gang plugin! A plugin which allows players to form gangs (stored in sql databases) which allows intra-gang communication and round benefits. 
### JBFS Timeout v1.1
- Added string sanitization for player names
### JBFS Gangs v1.0
- Added Gang plugin
- Players can create gangs, invite to gangs, promote ranks, use gang chat, and much more.
- Gangs stored in SQL database (required for plugin to function) and will persist across sessions/servers
- Gang tag functionality to allow players to display their gangs in chat
### JBFS v1.1
- Added LastRequest ConVar, which can be used to disable the LastRequest system entirely
- Added ConVars for Warden commands, so that they can be disabled. If disabled, a command will not show up in the warden menu. This will not disable the admin command equivalent.
- Setting marker time to 0 no longer disables the marker system, instead disabling the command will now disable markers
- Fixed timer issue for lr command on round start
- Improved logging and message displays for admin commands
- Added Timeout plugin! A rudamentary teamban system that uses an sql database to store bans. In this plugin, bans are done on a round basis, rather than a "number of minutes" basis. This is partially me pushing my own ideology, but it also makes more sense from a round-based game perspective.