# GoldHEN Plugins
GoldHEN Plugins for PlayStation 4.

## Plugins

#### Plugin Manager
Manage installed plugins.

```ini
; Load all plugins in `GoldHEN_plugins` section
[GoldHEN_plugins]
/data/GoldHEN/plugins/example.prx
/data/GoldHEN/plugins/example2.prx
```

#### No Share Blocks
Removes arbitrary limitations when using Share Button in Games.

#### Fliprate Remover
Removes framerate limit for games using system library `sceVideoOutSetFlipRate`.

#### Game Patch
Patches game before boot.

<details>
<summary>How To Use (Click to Expand)</summary>

- Download zip from [console-game-patches](https://github.com/illusion0001/console-game-patches/raw/gh-pages/patch1.zip)
- Extract it to `/data/GoldHEN/`
- Patches can be enabled/disabled in following apps:
  - GoldHEN Patch Settings
  - GoldHEN Cheat Manager
  - Itemzflow
- Run your game.

</details>

#### Button Swap
Swap X and O around on Asia Region console.

#### Application File Redirector
Redirect file calls to `/data/`

<details>
<summary>How To Use (Click to Expand)</summary>

- Create a folder in `/data`
  - `/data/GoldHEN/AFR/(title id)/`
- Place files into newly created `(title id)` folder
  - Example for `CUSA00001` `/app0/hello_afr.txt` -> `/data/GoldHEN/AFR/CUSA00001/hello_afr.txt`
- Run your game.

</details>

# Building

## Requirements

- [LLVM 10+](https://llvm.org/)
- [OpenOrbis SDK](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
- [GoldHEN Plugin SDK](https://github.com/GoldHEN/GoldHEN_Plugins_SDK)

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
3. Run `./build.sh` You can find built binaries in `bin/plugins`.

### Windows

1. Set environment variable to OpenOrbis and GoldHEN SDK

2. Run `build_static.bat` in GoldHEN SDK repository to build the library statically.

3. Run `build.bat` to build all the modules at once.

## Libraries used

- [tiny-json](https://github.com/rafagafe/tiny-json)
