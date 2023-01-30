#ifndef _PAD_H
#define _PAD_H

#include <stdbool.h>

/*
 TOUCH Pad Button
 -----------
| L1     R1 |
|           |
| L2     R2 |
 ----------
*/

typedef enum {
    TOUCH_L1,
    TOUCH_R1,
    TOUCH_L2,
    TOUCH_R2,
    BUTTON_L3,
    BUTTON_R3,
    BUTTON_OPTIONS,
    BUTTON_UP,
    BUTTON_RIGHT,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_L2,
    BUTTON_R2,
    BUTTON_L1,
    BUTTON_R1,
    BUTTON_TRIANGLE,
    BUTTON_CIRCLE,
    BUTTON_CROSS,
    BUTTON_SQUARE,
    BUTTON_TOUCH_PAD,
    BUTTON_INTERCEPTED,
    BUTTON_MAPPING_MAX
} ButtonMapping;

typedef enum ScePadButtonDataOffset {
    SCE_PAD_BUTTON_L3 = 0x00000002,
    SCE_PAD_BUTTON_R3 = 0x00000004,
    SCE_PAD_BUTTON_OPTIONS = 0x00000008,
    SCE_PAD_BUTTON_UP = 0x00000010,
    SCE_PAD_BUTTON_RIGHT = 0x00000020,
    SCE_PAD_BUTTON_DOWN = 0x00000040,
    SCE_PAD_BUTTON_LEFT = 0x00000080,
    SCE_PAD_BUTTON_L2 = 0x00000100,
    SCE_PAD_BUTTON_R2 = 0x00000200,
    SCE_PAD_BUTTON_L1 = 0x00000400,
    SCE_PAD_BUTTON_R1 = 0x00000800,
    SCE_PAD_BUTTON_TRIANGLE = 0x00001000,
    SCE_PAD_BUTTON_CIRCLE = 0x00002000,
    SCE_PAD_BUTTON_CROSS = 0x00004000,
    SCE_PAD_BUTTON_SQUARE = 0x00008000,
    SCE_PAD_BUTTON_TOUCH_PAD = 0x00100000,
    SCE_PAD_BUTTON_INTERCEPTED = 0x80000000,
} ScePadButtonDataOffset;

typedef enum {
    PAD_VIRATION_INTENSITY_STRONG,
    PAD_VIRATION_INTENSITY_MEDIUM,
    PAD_VIRATION_INTENSITY_WEAK,
    PAD_VIRATION_INTENSITY_OFF
} VirationIntensity;

typedef struct ScePadAnalogStick {
    uint8_t x;
    uint8_t y;
} ScePadAnalogStick;

typedef struct ScePadAnalogButtons {
    uint8_t l2;
    uint8_t r2;
    uint8_t padding[2];
} ScePadAnalogButtons;

typedef struct SceFQuaternion {
    float x, y, z, w;
} SceFQuaternion;

typedef struct SceFVector3 {
    float x, y, z;
} SceFVector3;

typedef struct ScePadTouch {
    uint16_t x;
    uint16_t y;
    uint8_t id;
    uint8_t reserve[3];
} ScePadTouch;

typedef struct ScePadTouchData {
    uint8_t touchNum;
    uint8_t reserve[3];
    uint32_t reserve1;
    ScePadTouch touch[2];
} ScePadTouchData;

typedef struct ScePadExtensionUnitData {
    uint32_t extensionUnitId;
    uint8_t reserve[1];
    uint8_t dataLength;
    uint8_t data[10];
} ScePadExtensionUnitData;

typedef struct ScePadData {
    uint32_t buttons;
    ScePadAnalogStick leftStick;
    ScePadAnalogStick rightStick;
    ScePadAnalogButtons analogButtons;
    SceFQuaternion orientation;
    SceFVector3 acceleration;
    SceFVector3 angularVelocity;
    ScePadTouchData touchData;
    bool connected;
    uint64_t timestamp;
    ScePadExtensionUnitData extensionUnitData;
    uint8_t connectedCount;
    uint8_t reserve[2];
    uint8_t deviceUniqueDataLen;
    uint8_t deviceUniqueData[12];
} ScePadData;

typedef struct ScePadTouchPadInformation {
    float pixelDensity;
    struct {
        uint16_t x;
        uint16_t y;
    } resolution;
} ScePadTouchPadInformation;

typedef struct ScePadStickInformation {
    uint8_t deadZoneLeft;
    uint8_t deadZoneRight;
} ScePadStickInformation;

typedef enum {
    SCE_PAD_DEVICE_CLASS_INVALID = -1,
    SCE_PAD_DEVICE_CLASS_STANDARD = 0,
    SCE_PAD_DEVICE_CLASS_GUITAR = 1,
    SCE_PAD_DEVICE_CLASS_DRUM = 2,
    SCE_PAD_DEVICE_CLASS_DJ_TURNTABLE = 3,
    SCE_PAD_DEVICE_CLASS_DANCEMAT = 4,
    SCE_PAD_DEVICE_CLASS_NAVIGATION = 5,
    SCE_PAD_DEVICE_CLASS_STEERING_WHEEL = 6,
    SCE_PAD_DEVICE_CLASS_STICK = 7,
    SCE_PAD_DEVICE_CLASS_FLIGHT_STICK = 8,
    SCE_PAD_DEVICE_CLASS_GUN = 9,
} ScePadDeviceClass;

typedef struct ScePadControllerInformation {
    ScePadTouchPadInformation touchPadInfo;
    ScePadStickInformation stickInfo;
    uint8_t connectionType;
    uint8_t connectedCount;
    bool connected;
    ScePadDeviceClass deviceClass;
    uint8_t reserve[8];
} ScePadControllerInformation;

typedef struct ScePadVibrationParam {
    uint8_t largeMotor;
    uint8_t smallMotor;
} ScePadVibrationParam;

int scePadGetControllerInformation(int32_t handle, ScePadControllerInformation* pInfo);
int scePadRead(int handle, ScePadData* data, int count);
int scePadReadState(int handle, ScePadData* data);
int scePadReadExt(int handle, ScePadData* data, int count);
int scePadReadStateExt(int handle, ScePadData* data);
int scePadSetLightBar(int32_t handle, const void* pParam);
int scePadSetVibration(int32_t handle, const ScePadVibrationParam* pParam);

#endif
