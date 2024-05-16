# Jailbreak From Scratch
A hand made jailbreak plugin for Team Fortress 2, with more modern features.

## Opening blurb/credits
I made this plugin because I wanted to learn how to write sourcemod. Additionally, the repo of the current most popular jailbreak plugin, [Jailbreak Redux](https://github.com/Scags/TF2-Jailbreak-Redux) by Scags, has been archived as of May, 2023. This feels like a good time to release my own plugin.

I mentioned Jailbreak Redux, and this plugin was heavily inspired by it. I am a (somewhat former) jailbreak player, and I am very used to how Jailbreak Redux works, thus many of the functionalities of my plugin are similar. Additionally, as I was learning I had the source code of Redux open on the other monitor so I could understand how certain things are done. If you actually bother reading the code, you will notice many similarities. My biggest thanks to Scags for creating Redux, it is a plugin that has served me well for a long time and has modernized the old original plugin.

### Translation Credits
Big thanks to anyone who submits translation requests.
- Mr. Cai ([q1085909155](https://github.com/q1085909155)) - Chinese Translations
- Berke ([RealBerke](https://github.com/RealBerke)) - Turkish Translations

## Features
JBFS contains many interesting features. For the sake of not being redundant with redux, I will only include new and interesting features which are not a part of redux.

### Pros
1. Better map integration. Maps can mesh with the plugin using vscript (if installed on server), and the plugin does a better job of reading information from the map for things such as the cell doors, or LR spawn locations. Additionally, there is a feature that allows map makers to create completely custom map-specific LRs using named logic relays. Current vscript integration is fairly barebones and essential, however it can grow and be modified to allow 
2. More and better commands. There are more commands present in the plugin, especially for admins, that will make working with it a lot easier. There is granular permissioning for admin commands that will allow you to only give admins access to specific commands based on their admin flags.
3. Automatic mic checking. By default the plugin will check guards for microphones and will priority autobalance ones without microphones, as well as preventing guards without microphones from becoming warden (all configurable through cvars).
4. Better ammo integration. There are many parts of this plugin that trigger off of ammo pickup. Weapons such as the Bonk can be gated behind having ammo, as well as class effects such as scout double jumping and demoman charging.
5. Round based teamban plugin! More in line with my philosophy for jailbreak. Rather than expiring after a certain amount of minutes, a player stays guardbanned for a specific number of rounds that they participate in.
6. Gang plugin! Allows players to form gangs, have secret gang chats, earn points, and buy gang-wide buffs.
7. Built-in weapon search functionality, and surrender capability.

### Cons (current)
1. The plugin does not have a lot of developer integration, besides some barebones guard banning functionality. See wiki for more info. There is no support currently for third party LR plugins. Things such as Prophunt, VSH, or any other extra LRs are unsupported. The currently supported list is only 6 default ones, as well as the map LR functionality. As of v1.2, some more integration has been added, however still barebones.

## Information
Head over to the [Wiki](https://github.com/rsedxcftvgyhbujnkiqwe/TF2-Jailbreak-From-Scratch/wiki) for detailed information about everything I could think of with the plugin. Includes information about configuration files, cvars, as well as information for mappers.

## Installation
The installation guide can be found on the [Wiki](https://github.com/rsedxcftvgyhbujnkiqwe/TF2-Jailbreak-From-Scratch/wiki/Installation)

## Contributing
For feature requests, please submit an issue. For code contributions, such as features or bug fixes, I welcome pull requests, however there will probably be some discussion first.

Feel free to submit translation request PRs.
