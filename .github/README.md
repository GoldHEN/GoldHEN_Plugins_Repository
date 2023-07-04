# GoldHEN Plugins
Plugins allows you to customize your games like never before!

[![Build modules](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/actions/workflows/CI.yml/badge.svg?branch=main&event=push)](https://github.com/GoldHEN/GoldHEN_Plugins_Repository/actions/workflows/CI.yml)

## Features:

- Hook into Games before boot.
- Write your own code into games in C/C++.
- Hook and modify system functions.

## :warning: Disclaimer

While we make every effort to deliver high quality products, we do not guarantee that our products are free from defects. Our software is provided **as is** and you use the software at your own risk.

# Quick Start

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

### Force 1080p Display

Plugin filename: `force_1080p_display.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Reports to the app that it is connected to a 1080p display.

### Force 30 FPS

Plugin filename: `force_30_fps.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Force any game to run at 30 FPS.

### Fliprate Remover

Plugin filename: `fliprate_remover.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Removes framerate limit for games using system function `sceVideoOutSetFlipRate`.

### FrameTime Logger

Plugin filename: `frame_logger.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

  - Log frametime statistics.
  - Press `L3 + L1 + Triangle` to start capturing data.
  - Press `L3 + R3 + L1 + R1 + Square` to stop plugin.
  - Logs data to `/data/frame_logger/`.

### GamePad helper Plugin

Plugin filename: `gamepad_helper.prx`

Author(s):
- [jocover](https://github.com/jocover)

#### Features:
- Custom DeadZone settings
- Custom TouchPad Button
- Custom Button Mapping
- Custom Viration Intensity settings

#### Getting Started
Config File in `/data/GoldHEN/gamepad.ini`.

```ini
; Load gamepad config for any title

[default]
enableDeadZone=1
DeadZoneLeft=13
DeadZoneRight=13

enableCustomTouchPad=1
TOUCH_L2=BUTTON_L3
TOUCH_R2=BUTTON_TOUCH_PAD

; Load gamepad config for Playroom

[CUSA00001]
enableCustomButton=1
BUTTON_L3=BUTTON_CROSS

VirationIntensity=medium
```

### Configuration Options
#### DeadZone setting:
- enableDeadZone:`value: 0,1`
- DeadZoneLeft:` value: 0~128`
- DeadZoneRight:`value: 0~128`

#### Custom TouchPad setting:
- enableCustomTouchPad:  `value: 0,1`
#### Support TouchPad Button Mapping list

```
TOUCH_L1
TOUCH_R1
TOUCH_L2
TOUCH_R2
```

#### TouchPad Button Map

| **L1** | &emsp;&emsp;&emsp; | **R1** |
|--------|--------------------|--------|
| &emsp; | &emsp;             | &emsp; |
| **L2** | &emsp;&emsp;&emsp; | **R2** |

#### Custom Button setting:
- enableCustomButton: `value: 0,1`
##### Support Button Mapping list

```
BUTTON_OPTIONS
BUTTON_UP
BUTTON_RIGHT
BUTTON_DOWN
BUTTON_LEFT
BUTTON_L3
BUTTON_R3
BUTTON_L2
BUTTON_R2
BUTTON_L1
BUTTON_R1
BUTTON_TRIANGLE
BUTTON_CIRCLE
BUTTON_CROSS
BUTTON_SQUARE
BUTTON_TOUCH_PAD
```

#### Viration Intensity setting:
- VirationIntensity: `options: off,weak,medium,strong`

### Game Patch

Plugin filename: `game_patch.prx`

Author(s):
- [illusion](https://github.com/illusion0001)

Patches game before boot. Using patches from the [Patch Repository](https://github.com/GoldHEN/GoldHEN_Patch_Repository)

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

- [mxml](https://github.com/bucanero/mxml)

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
3. Run `make` in the root of the repository or `make` individually; built binaries can be found in `bin/plugins`.

<!-- Win32 build script not up to date-->
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
