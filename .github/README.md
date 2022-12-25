# GoldHEN Plugins
Plugins allows you to customize your games like never before!

[![Build modules](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/actions/workflows/CI.yml/badge.svg?branch=main&event=push)](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/actions/workflows/CI.yml)

## Features:

- Hook into Games before boot.
- Write your own code into games in C/C++.
- Hook and modify system functions.

## :warning: Disclaimer

While we make every effort to deliver high quality products, we do not guarantee that our products are free from defects. Our software is provided **as is** and you use the software at your own risk.

# Getting Started

- Load [GoldHEN 2.3](https://github.com/GoldHEN/GoldHEN/releases/latest) or newer on your PS4.
- Enable option to load plugins in Plugins Menu.
- Download plugins from [Release Page](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/releases/latest).
- Extract downloaded plugins into `/data/GoldHEN/plugins/`.
- Add plugins you want to load into `/data/GoldHEN/plugins.ini`.
  - Per game plugins sections are recommended over putting everything in `default`.

```ini
; Load plugins for any title

[default]
/data/GoldHEN/plugins/game_patch.prx

; Load plugins only for Playroom

[CUSA00001]
/data/GoldHEN/plugins/afr.prx
/data/GoldHEN/plugins/no_share_watermark.prx
```

## Plugins

### AFR (Application File Redirector)

Plugin filename: `afr.prx`

Author(s):
- [SiSTR0](https://github.com/SiSTR0)
- [jocover](https://github.com/jocover)

Redirect file calls to `/data/`

<details>
<summary>How To Use (Click to Expand)</summary>

- Create a folder in `/data`
  - `/data/GoldHEN/AFR/(title id)/`
- Place files into newly created `(title id)` folder
  - Example for `CUSA00001` `/app0/hello_afr.txt` -> `/data/GoldHEN/AFR/CUSA00001/hello_afr.txt`
- Run your game.

</details>

### Button Swap

Plugin filename: `button_swap.prx`

Author(s):
- [jocover](https://github.com/jocover)

Swap X and O around on Asia Region console.

### Fliprate Remover

Plugin filename: `fliprate_remover.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Removes framerate limit for games using system function `sceVideoOutSetFlipRate`.

### Game Patch

Plugin filename: `game_patch.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Patches game before boot.

<details>
<summary>How To Use (Click to Expand)</summary>

#### Manual Installation
- Download [patch zip](https://github.com/GoldHEN/GoldHEN_Patch_Repository/raw/gh-pages/patch1.zip)
- Extract it to `/data/GoldHEN/`

#### Easy Installation
- Patches can be configured, install/update via:
  - [GoldHEN Cheat Manager](https://github.com/GoldHEN/GoldHEN_Cheat_Manager/releases/latest)
  - [Itemzflow Game Manager](https://github.com/LightningMods/Itemzflow)
- Run your game.

</details>

##### Libraries used

- [tiny-json](https://github.com/rafagafe/tiny-json)

### No Share Blocks

Plugin filename: `no_share_watermark.prx`

Author(s):
- [illusion](https://github.com/illusion0001)
- [nik](https://github.com/nkrapivin)

Removes image watermark, video and screenshot blocks from games.

# Building

## Requirements

- [LLVM 10+](https://llvm.org/)
- [OpenOrbis SDK](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
- [GoldHEN Plugin SDK](https://github.com/GoldHEN/GoldHEN_Plugins_SDK)
- Submodules checkout with `git submodule update --init`

## Build

### Linux

1. Export path to OpenOrbis SDK.

```
export OO_PS4_TOOLCHAIN=[directory of installation]
```
2. Export path to GoldHEN SDK.

```
export GOLDHEN_SDK=[directory of installation]
```
3. Run `./build.sh` or `make` individually; built binaries can be found in `bin/plugins`.

# Troubleshooting log crashes

If you ran into crashes related to print logging (usually during VA args assignment clearing stack) you can try building with printf `./build.sh -printf`.

### Windows

1. Set environment variable to OpenOrbis and GoldHEN SDK
2. Run `build_static.bat` in GoldHEN SDK repository to build the library statically.
3. Run `build.bat` to build all the modules at once.

# Coded by:

- [bucanero](https://github.com/bucanero)
- [ctn123](https://github.com/ctn123)
- [illusion](https://github.com/illusion0001)
- [jocover](https://github.com/jocover)
- [nkrapivin](https://github.com/nkrapivin)
- [OSM](https://github.com/OSM-Made)
- [SiSTRo](https://github.com/SiSTR0)
