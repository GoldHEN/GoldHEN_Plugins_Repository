# GoldHEN Plugins

GoldHEN Plugins for PlayStation 4.

## Plugins

#### [No Share Watermark](/plugin-src/no-share-watermark)

Removes watermark from PS4 Screenshots when using Share Button.

#### [Fliprate Remover](/plugin-src/no-share-watermark)

Removes framerate limit for games using system library `sceVideoOutSetFlipRate`.

#### [Game Patch](/plugin-src/game-patch)

Patches game before boot.

#### [Button Swap](/plugin-src/button-swap)

Swap X and O around on Asia Region console.

#### [Application File Redirector](/plugin-src/AFS)

Redirect file calls to `/data/`

# Building

## Requirements

- [GoldHEN Plugin SDK](https://github.com/GoldHEN/)
- [OpenOrbis SDK](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)

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
