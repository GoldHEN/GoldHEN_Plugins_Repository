# GoldHEN Plugins
GoldHEN Plugins for PlayStation 4.

[![Build modules](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/actions/workflows/CI.yml/badge.svg?branch=main&event=push)](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/actions/workflows/CI.yml)

# Getting Started

Create a file config file: `/data/GoldHEN/plugins.ini`

Upload plugin files to `/data/GoldHEN/plugins/`

Example ini usage:

```ini
; Load plugins under specific Title ID `CUSA12345`
[CUSA12345]
/data/GoldHEN/plugins/example34.prx
/data/GoldHEN/plugins/example138.prx

; Load plugins in `default` section regardless of Title ID
[default]
/data/GoldHEN/plugins/example.prx
/data/GoldHEN/plugins/example2.prx
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

Plugin filename: `button-swap.prx`

Author(s):
- [jocover](https://github.com/jocover)

Swap X and O around on Asia Region console.

### Fliprate Remover

Plugin filename: `fliprate-remover.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Removes framerate limit for games using system function `sceVideoOutSetFlipRate`.

### Game Patch

Plugin filename: `game-patch.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Patches game before boot.

<details>
<summary>How To Use (Click to Expand)</summary>

- Download zip from [GoldHEN_Patch_Repository](https://github.com/GoldHEN/GoldHEN_Patch_Repository/raw/gh-pages/patch1.zip)
- Extract it to `/data/GoldHEN/`
- Patches can be configured via:
  - GoldHEN Patch Settings
  - GoldHEN Cheat Manager
  - Itemzflow Game Manager
- Run your game.

</details>

##### Libraries used

- [tiny-json](https://github.com/rafagafe/tiny-json)

<!--

Users don't need this unless for development.

### Plugin Loader

Plugin filename: `plugin-loader.prx`

Author(s):
- [Ctn123](https://github.com/Ctn123)
- [illusion](https://github.com/illusion0001)

Manage installed plugins.

Create a file `/data/GoldHEN/plugins.ini`

Example usage:

```ini
; Load plugins under specific Title ID `CUSA12345`
[CUSA12345]
/data/GoldHEN/plugins/example34.prx
/data/GoldHEN/plugins/example138.prx

; Load plugins in `default` section regardless of Title ID
[default]
/data/GoldHEN/plugins/example.prx
/data/GoldHEN/plugins/example2.prx
```

-->

### No Share Blocks

Plugin filename: `no-share-watermark.prx`

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

### Windows

1. Set environment variable to OpenOrbis and GoldHEN SDK
2. Run `build_static.bat` in GoldHEN SDK repository to build the library statically.
3. Run `build.bat` to build all the modules at once.
