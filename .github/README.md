# GoldHEN Plugins
GoldHEN Plugins for PlayStation 4.

## Plugins

#### No Share Watermark
Removes watermark from PS4 Screenshots when using Share Button.

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
  - `/data/GoldHEN/AFR/(title id)/app0`
- Place files into newly created `app0` folder
  - Example for `CUSA00001` `/app0/boot.txt` -> `/data/GoldHEN/AFR/CUSA00001/app0/boot.txt`
- Run your game.

</details>

# Building

## Requirements

- [GoldHEN Plugin SDK](https://github.com/GoldHEN/)
- [OpenOrbis SDK](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)

## Libraries used

- [tiny-json](https://github.com/rafagafe/tiny-json)

## Setup

1. Export path to OpenOrbis SDK.

```
export OO_PS4_TOOLCHAIN=[directory of installation]
```
2. Export path to GoldHEN SDK.

```
export GOLDHEN_SDK=[directory of installation]
```
3. Run `./build.sh` You can find built binaries in `bin/plugins`.
