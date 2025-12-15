#pragma once

#include <cstdint>

// GTA IV Input System
// Handles controller input translation for GTA IV controls
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
    
    // Triggers (analog) - LT/RT thresholds
    constexpr uint8_t LT_THRESHOLD = 30;       // On foot: Aim/Lock-on | In car: Brake/Reverse
    constexpr uint8_t RT_THRESHOLD = 30;       // On foot: Shoot | In car: Accelerate
    
    // Stick deadzones
    constexpr int16_t LEFT_STICK_DEADZONE = 7849;   // ~24% of max
    constexpr int16_t RIGHT_STICK_DEADZONE = 8689;  // ~26.5% of max
}

struct GTA4InputState
{
    uint16_t buttons;
    int16_t leftStickX;
    int16_t leftStickY;
    int16_t rightStickX;
    int16_t rightStickY;
    uint8_t leftTrigger;
    uint8_t rightTrigger;
    
    bool isAiming;
    bool isShooting;
    bool isSprinting;
};

// Input system functions
void GTA4_InitInput();
void GTA4_UpdateInputState();
void GTA4_UpdatePreviousState();
const GTA4InputState& GTA4_GetInputState();

// Button state queries
bool GTA4_IsButtonDown(uint16_t button);
bool GTA4_IsButtonPressed(uint16_t button);
bool GTA4_IsButtonReleased(uint16_t button);

// Vibration
void GTA4_SetVibration(uint16_t leftMotor, uint16_t rightMotor);
void GTA4_StopVibration();

// Hook for game's input reading
extern "C" uint32_t GTA4_XInputGetState(uint32_t dwUserIndex, void* pState);
