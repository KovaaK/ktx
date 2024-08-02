# KTXSmash: a QuakeWorld server modification

This is a QuakeWorld server mod meant to run with mvdsv that adds several game modes to KTX with smash style gameplay.  

## Description ##

In-game, your armor number is replaced with your current knockback %, and when you do damage to an opponent your UI health number shows your target's current knockback %.  
Starting at 0% on spawn, you take very little knockback.  
Quake damage numbers add 1/4th of the normal value (a direct rocket adds 110/4 = 27%, a single LG cell adds 7.5% and can stack 75%/s max DPS).  
By 100% you take normal knockback, and by 200% you're taking double knockback.  
LG Knockback is nerfed above certain percents so that Rocket Launcher gets to shine for most killing blows.  
Axe is stupid strong, but good luck landing a hit.  

The only way to die is to get knocked outside of the level or be telefragged.  
When you get knocked off the level, use Quake air physics to redirect your momentum and try to recover.  
Press jump in the air to double jump, and aim at a ledge to ledge-grab (then jump again to get back on the level).  

## Game modes ##
All of the new custom modes enable Smash style gameplay as above.  Hitting a teammate won't increase their percentage, but it will knock them back scaled by their %.

Games with "packman" enabled will have a glowing green backpack that spawns.  Collecting it gives you 1 point every 5s, but it also damages you over time (25% every time it gives a point).  In team and duel modes the packman's killer receives the pack; in FFA or for team-kills the pack respawns randomly after the carrier dies.

In prematch, the following commands can be entered to change the server's mode:
* **smashmodeffa** - Smash Free-For-All, 10 fraglimit
* **smashmode1on1** - Smash Duel, 10 fraglimit
* **smashmodearena** - Winner stays rotating duels starting from 0%, 10 fraglimit
* **smashmodetdm** - Smash Team Deathmatch, 5 minute timelimit
* **smashmodewipeout** - Smash Wipeout, round-based (bo9) wherein you win a round by having all players on the enemy team dead at the same time, and each death increases your respawn timer.
* **smashpackffa** - Packman Free-For-All, 10 minute timelimit or 50 fraglimit
* **smashpack1on1** - Packman Duel, 10 minute timelimit or 50 fraglimit
* **smashpacktdm** - Packman Team Deathmatch, 5 minute timelimit or 50 fraglimit

## 9 maps included ##
Originals: **qsm-ring, qsm-push**  
Mods to existing maps: **qsm-aero, qsm-cata, qsm-dm2, qsm-dm3, qsm-dm4, qsm-dm6, qsm-pkeg**

The only real requirement for playable maps is to have trigger_hurt boxes that do enough damage to kill the player when they are shoved into them.  We've found that making the surrounding killboxes far and wide play best.  If there is a ceiling killbox, try at least 900 units above the top floor.  Or be creative and you do you, I'd love to see more cool maps for this!

## Credits ##
KovaaK - smash mod idea, code (original mod made without KTX in 2004-2005)  
Nick - packman idea, running servers  
Mgli - maps  
Ckap - hardest man to chase in packman, running servers  
Toxic - packman double jump nerf idea, <3  
Everyone who has played - thank you for your input, feedback, reactions, and everything.  It's been a joy experiencing Quake like this with you!  


Original KTX README.md follows:



# KTX: a QuakeWorld server modification
![KTX Logo](https://raw.githubusercontent.com/QW-Group/ktx/master/resources/logo/ktx.png)

**[KTX][ktx]** (Kombat Teams eXtreme) is a popular **QuakeWorld** server modification, adding numerous features to the core features of the server.

Although it had been developed to be **Quakeworld** server agnostic, it has over the years been developed very close to **[MVDSV][mvdsv]** to which it has become an extent, thus compatibility with other **Quakeworld** servers might not have been maintained.

## Getting Started

The following instructions will help you get **[KTX][ktx]** installed on a running **[MVDSV][mvdsv]** server using prebuilt binaries. Details on how to compile your own **[KTX][ktx]** binary will also be included to match specific architectures or for development purposes.

## Supported architectures

The following architectures are fully supported by **[KTX][ktx]** and are available as prebuilt binaries:
* Linux amd64 (Intel and AMD 64-bits processors)
* Linux i686 (Intel and AMD 32-bit processors)
* Linux aarch (ARM 64-bit processors)
* Linux armhf (ARM 32-bit processors)
* Windows x64 (Intel and AMD 64-bits processors)
* Windows x86 (Intel and AMD 32-bit processors)

## Prebuilt binaries

You can find the prebuilt binaries on [this download page][ktx-builds].

## Prerequisites

**[KTX][ktx]** is a server mod and won't run without a proper **Quakeworld** server set up. **[MVDSV][mvdsv]** is the recommended one, but **[FTE][fte]** might work as well (unconfirmed with current code).

## Installing

For more detailed information we suggest looking at the [nQuake server][nquake-linux], which uses **[MVDSV][mvdsv]** and **[KTX][ktx]** as **QuakeWorld** server.

## Building binaries

### Build from source with CMake

Assuming you have installed essential build tools and ``CMake``
```bash
mkdir build && cmake -B build . && cmake --build build
```
Build artifacts would be inside ``build/`` directory, for unix like systems it would be ``qwprogs.so``.

You can also use ``build_cmake.sh`` script, it mostly suitable for cross compilation
and probably useless for experienced CMake user.
Some examples:
```
./build_cmake.sh linux-amd64
```
should build KTX for ``linux-amd64`` platform, release version, check [cross-cmake](tools/cross-cmake) directory for all platforms

```
B=Debug ./build_cmake.sh linux-amd64
```
should build KTX for linux-amd64 platform with debug

```
V=1 B=Debug ./build_cmake.sh linux-amd64
```
should build KTX for linux-amd64 platform with debug, verbose (useful if you need validate compiler flags)

```
V=1 B=Debug BOT_SUPPORT=OFF ./build_cmake.sh linux-amd64
```

same as above but compile without bot support

```
G="Unix Makefiles" ./build_cmake.sh linux-amd64
```

force CMake generator to be unix makefiles

```
./build_cmake.sh linux-amd64 qvm
```

build KTX for ``linux-amd64`` and ``QVM`` version, you can provide
any platform combinations.

## Versioning

For the versions available, see the [tags on this repository][ktx-tags].

## Authors

(Listed by last name alphabetic order)

* **Ivan** *"qqshka"* **Bolsunov**
* **Dominic** *"oldman"* **Evans**
* **Anton** *"tonik"* **Gavrilov**
* **Andrew** *"ult"* **Grondalski**
* **Paul Klumpp**
* **Niclas** *"empezar"* **Lindström**
* **Dmitry** *"disconnect"* **Musatov**
* **Peter** *"meag"* **Nicol**
* **Andreas** *"molgrum"* **Nilsson**
* **Alexandre** *"deurk"* **Nizoux**
* **Tero** *"Renzo"* **Parkkonen**
* **Joseph** *"bogojoker"* **Pecoraro**
* **Michał** *"\_KaszpiR\_"* **Sochoń**
* **Jonny** *"dimman"* **Svärd**
* **Vladimir** *"VVD"* **Vladimirovich**
* **Florian** *"Tuna"* **Zwoch**

## Code of Conduct

We try to stick to our code of conduct when it comes to interaction around this project. See the [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) file for details.

## License

This project is licensed under the GPL-2.0 License - see the [LICENSE.md](LICENSE.md) file for details.

## Acknowledgments

* Thanks to kemiKal, Cenobite, Sturm and Fang for Kombat teams 2.21 which has served as a base for **[KTX][ktx]**.
* Thanks to **Jon "bps" Cednert** for the **[KTX][ktx]** logo.
* Thanks to the fine folks on [Quakeworld Discord][discord-qw] for their support and ideas.

[ktx]: https://github.com/QW-Group/ktx
[ktx-tags]: https://github.com/QW-Group/ktx/tags
[ktx-builds]: https://builds.quakeworld.nu/ktx
[mvdsv]: https://github.com/QW-Group/mvdsv
[nquake-linux]: https://github.com/nQuake/server-linux
[discord-qw]: http://discord.quake.world/
