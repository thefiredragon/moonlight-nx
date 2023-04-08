//
//  InputManager.hpp
//  Moonlight
//
//  Created by Даниил Виноградов on 27.05.2021.
//

#pragma once

#include "Singleton.hpp"
#include "keyboard_view.hpp"
#include <borealis.hpp>
#include <optional>

// Moonlight ready gamepad
struct GamepadState {
    short buttonFlags = 0;
    unsigned char leftTrigger = 0;
    unsigned char rightTrigger = 0;
    short leftStickX = 0;
    short leftStickY = 0;
    short rightStickX = 0;
    short rightStickY = 0;

    bool is_equal(GamepadState other) {
        return buttonFlags == other.buttonFlags &&
               leftTrigger == other.leftTrigger &&
               rightTrigger == other.rightTrigger &&
               leftStickX == other.leftStickX &&
               leftStickY == other.leftStickY &&
               rightStickX == other.rightStickX &&
               rightStickY == other.rightStickY;
    }
};

struct MouseStateS {
    float scroll_y;
    bool l_pressed;
    bool m_pressed;
    bool r_pressed;
};

class MoonlightInputManager : public Singleton<MoonlightInputManager> {
  public:
    MoonlightInputManager();
    void dropInput();
    void handleInput();
    void handleRumble(unsigned short controller, unsigned short lowFreqMotor,
                      unsigned short highFreqMotor);
    void updateTouchScreenPanDelta(brls::PanGestureStatus panStatus);
    void reloadButtonMappingLayout();
    void leftMouseClick();
    void rightMouseClick();

  private:
    GamepadState lastGamepadStates[GAMEPADS_MAX];
    brls::ControllerButton mappingButtons[brls::_BUTTON_MAX];
    std::optional<brls::PanGestureStatus> panStatus;
    bool inputDropped = false;

    brls::ControllerState mapController(brls::ControllerState controller);
    int glfwKeyToVKKey(brls::BrlsKeyboardScancode key);

    GamepadState getControllerState(int controllerNum, bool specialKey);
    void handleControllers(bool specialKey);

    short controllersToMap();
};
