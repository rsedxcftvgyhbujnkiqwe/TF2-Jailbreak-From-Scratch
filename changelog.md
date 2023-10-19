### v1.1
- Added LastRequest ConVar, which can be used to disable the LastRequest system entirely
- Added ConVars for Warden commands, so that they can be disabled. If disabled, a command will not show up in the warden menu. This will not disable the admin command equivalent.
- Setting marker time to 0 no longer disables the marker system, instead disabling the command will now disable markers
- Fixed timer issue for lr command on round start
- Improved logging and message displays for admin commands
- Added Timeout plugin! A rudamentary teamban system that uses an sql database to store bans. In this plugin, bans are done on a round basis, rather than a "number of minutes" basis. This is partially me pushing my own ideology, but it also makes more sense from a round-based game perspective.
