# GamePad helper Plugin

## Features:
- Custom DeadZone settings
- Custom TouchPad Button
- Custom Button Mapping
- Custom Viration Intensity settings

# Getting Started
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

# Configuration Options
## DeadZone setting:
- enableDeadZone:`value: 0,1`
- DeadZoneLeft:` value: 0~128`
- DeadZoneRight:`value: 0~128`

## Custom TouchPad setting:
- enableCustomTouchPad:  `value: 0,1`
#### Support TouchPad Button Mapping list
```
TOUCH_L1
TOUCH_R1
TOUCH_L2
TOUCH_R2
```

#### TouchPad Button Map

| **L1**  | &emsp;&emsp;&emsp;  | **R1** |
| :------------: | :------------: | :------------: |
| &emsp;  | &emsp;  | &emsp;  | 
|  **L2** | &emsp;&emsp;&emsp;  | **R2**  |



## Custom Button setting:
- enableCustomButton:` value: 0,1`
#### Support Button Mapping list
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

## Viration Intensity setting:
- VirationIntensity:` options: off,weak,medium,strong`

