// GTA IV Input Remapping and Patches
// Handles controller input translation for GTA IV controls

#include <stdafx.h>
#include <hid/hid.h>
#include <user/config.h>
#include <os/logger.h>
#include <kernel/xdm.h>

// GTA IV Button Mappings (Xbox 360 layout)
// Based on official GTA IV Xbox 360 control scheme
namespace GTA4Input
{
    // Face buttons (XINPUT standard bit positions)
    constexpr uint16_t BUTTON_A = 0x1000;      // On foot: Sprint | In car: Handbrake
    constexpr uint16_t BUTTON_B = 0x2000;      // On foot: Melee/Grab | In car: Camera view
    constexpr uint16_t BUTTON_X = 0x4000;      // On foot: Jump/Climb | In car: Drive-by aim
    constexpr uint16_t BUTTON_Y = 0x8000;      // On foot: Enter vehicle/Attack | In car: Exit vehicle
    
    // D-Pad
    constexpr uint16_t DPAD_UP = 0x0001;       // Phone
    constexpr uint16_t DPAD_DOWN = 0x0002;     // Phone down/Skip call
    constexpr uint16_t DPAD_LEFT = 0x0004;     // Previous weapon/radio station
    constexpr uint16_t DPAD_RIGHT = 0x0008;    // Next weapon/radio station
    
    // Shoulders and sticks
    constexpr uint16_t LB = 0x0100;            // On foot: Hail taxi | In car: Shoot
    constexpr uint16_t RB = 0x0200;            // On foot: Take cover | In car: Handbrake
    constexpr uint16_t LSTICK = 0x0040;        // Crouch/Duck (press left stick)
    constexpr uint16_t RSTICK = 0x0080;        // Look behind (press right stick)
    
    // Start/Back
    constexpr uint16_t START = 0x0010;         // Pause Menu
    constexpr uint16_t BACK = 0x0020;          // Camera select
    
    // Triggers (analog) - LT/RT
    constexpr uint8_t LT_THRESHOLD = 30;       // On foot: Aim/Lock-on | In car: Brake/Reverse
    constexpr uint8_t RT_THRESHOLD = 30;       // On foot: Shoot | In car: Accelerate
    
    // Stick deadzones (GTA IV uses fairly standard deadzones)
    constexpr int16_t LEFT_STICK_DEADZONE = 7849;   // ~24% of max
    constexpr int16_t RIGHT_STICK_DEADZONE = 8689;  // ~26.5% of max
}

// Current input state for GTA IV
struct GTA4InputState
{
    uint16_t buttons = 0;
    int16_t leftStickX = 0;
    int16_t leftStickY = 0;
    int16_t rightStickX = 0;
    int16_t rightStickY = 0;
    uint8_t leftTrigger = 0;
    uint8_t rightTrigger = 0;
    
    bool isAiming = false;
    bool isShooting = false;
    bool isSprinting = false;
};

static GTA4InputState g_gta4InputState;

// Dead zone handling - uses GTA IV standard deadzones
static int16_t ApplyDeadzone(int16_t value, int16_t deadzone = GTA4Input::LEFT_STICK_DEADZONE)
{
    if (abs(value) < deadzone)
        return 0;
    
    // Scale remaining range to full range
    int16_t sign = value > 0 ? 1 : -1;
    int32_t magnitude = abs(value) - deadzone;
    int32_t scaledMax = 32767 - deadzone;
    
    return static_cast<int16_t>(sign * (magnitude * 32767 / scaledMax));
}

// Update GTA IV input state from HID
void GTA4_UpdateInputState()
{
    XAMINPUT_STATE state;
    if (hid::GetState(0, &state) != ERROR_SUCCESS)
    {
        memset(&g_gta4InputState, 0, sizeof(g_gta4InputState));
        return;
    }
    
    auto& pad = state.Gamepad;
    auto& gta = g_gta4InputState;
    
    // Buttons
    gta.buttons = pad.wButtons;
    
    // Analog sticks with deadzone
    gta.leftStickX = ApplyDeadzone(pad.sThumbLX);
    gta.leftStickY = ApplyDeadzone(pad.sThumbLY);
    gta.rightStickX = ApplyDeadzone(pad.sThumbRX, 8689); // Right stick has larger deadzone
    gta.rightStickY = ApplyDeadzone(pad.sThumbRY, 8689);
    
    // Triggers
    gta.leftTrigger = pad.bLeftTrigger;
    gta.rightTrigger = pad.bRightTrigger;
    
    // Derived states
    gta.isAiming = pad.bLeftTrigger > GTA4Input::LT_THRESHOLD;
    gta.isShooting = pad.bRightTrigger > GTA4Input::RT_THRESHOLD;
    gta.isSprinting = (pad.wButtons & GTA4Input::BUTTON_A) != 0;
}

// Get current GTA IV input state
const GTA4InputState& GTA4_GetInputState()
{
    return g_gta4InputState;
}

// Check if a button is pressed
bool GTA4_IsButtonDown(uint16_t button)
{
    return (g_gta4InputState.buttons & button) != 0;
}

// Check if a button was just pressed (requires previous state tracking)
static uint16_t g_prevButtons = 0;

bool GTA4_IsButtonPressed(uint16_t button)
{
    bool wasDown = (g_prevButtons & button) != 0;
    bool isDown = (g_gta4InputState.buttons & button) != 0;
    return isDown && !wasDown;
}

bool GTA4_IsButtonReleased(uint16_t button)
{
    bool wasDown = (g_prevButtons & button) != 0;
    bool isDown = (g_gta4InputState.buttons & button) != 0;
    return !isDown && wasDown;
}

void GTA4_UpdatePreviousState()
{
    g_prevButtons = g_gta4InputState.buttons;
}

// Vibration/Rumble support
void GTA4_SetVibration(uint16_t leftMotor, uint16_t rightMotor)
{
    XAMINPUT_VIBRATION vibration;
    vibration.wLeftMotorSpeed = leftMotor;
    vibration.wRightMotorSpeed = rightMotor;
    hid::SetState(0, &vibration);
}

void GTA4_StopVibration()
{
    GTA4_SetVibration(0, 0);
}

// Input remapping hooks (to be connected to game functions)
// These will intercept GTA IV's input reading and provide our translated input

// Hook for XInputGetState - intercepts game's input reading
extern "C" uint32_t GTA4_XInputGetState(uint32_t dwUserIndex, void* pState)
{
    if (dwUserIndex != 0 || !pState)
        return ERROR_DEVICE_NOT_CONNECTED;
    
    GTA4_UpdateInputState();
    
    // Copy our processed state to the game's expected format
    auto* gameState = reinterpret_cast<XAMINPUT_STATE*>(pState);
    
    XAMINPUT_STATE hostState;
    if (hid::GetState(0, &hostState) == ERROR_SUCCESS)
    {
        *gameState = hostState;
        return ERROR_SUCCESS;
    }
    
    return ERROR_DEVICE_NOT_CONNECTED;
}

// Initialize GTA IV input system
void GTA4_InitInput()
{
    LOGN("GTA IV Input system initialized");
    memset(&g_gta4InputState, 0, sizeof(g_gta4InputState));
    g_prevButtons = 0;
}
