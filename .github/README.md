# GoldHEN Plugins
GoldHEN Plugins for PlayStation 4.

## Plugins
#### Application File Redirector

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

#### Button Swap

Author(s):
- [jocover](https://github.com/jocover)

Swap X and O around on Asia Region console.

#### Fliprate Remover

Author(s):
- [illusion](https://github.com/illusion0001)

Removes framerate limit for games using system library `sceVideoOutSetFlipRate`.

#### Game Patch

Author(s):
- [illusion](https://github.com/illusion0001)

Patches game before boot.

##### Libraries used

- [tiny-json](https://github.com/rafagafe/tiny-json)

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

#### Plugin Manager

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

#### No Share Blocks

Author(s):
- [illusion](https://github.com/illusion0001)
- [nik](https://github.com/nkrapivin)

Removes arbitrary limitations when using Share Button in Games.

# Building

## Requirements

- [LLVM 10+](https://llvm.org/)
- [OpenOrbis SDK](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain)
- [GoldHEN Plugin SDK](https://github.com/GoldHEN/GoldHEN_Plugins_SDK)
- Submodules checkout `git submodule update --init`

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
