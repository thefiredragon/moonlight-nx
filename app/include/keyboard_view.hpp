//
//  keyboard_view.hpp
//  Moonlight
//
//  Created by Даниил Виноградов on 14.06.2021.
//

#pragma once

#include <borealis.hpp>

enum KeyboardKeys
{
    VK_BACK, VK_ESCAPE, VK_KEY_0, VK_KEY_1, VK_KEY_2, VK_KEY_3, VK_KEY_4, VK_KEY_5, VK_KEY_6, VK_KEY_7,
    VK_KEY_8, VK_KEY_9, VK_KEY_A, VK_KEY_B, VK_KEY_C, VK_KEY_D, VK_KEY_E, VK_KEY_F, VK_KEY_G, VK_KEY_H,
    VK_KEY_I, VK_KEY_J, VK_KEY_K, VK_KEY_L, VK_KEY_M, VK_KEY_N, VK_KEY_O, VK_KEY_P, VK_KEY_Q, VK_KEY_R,
    VK_KEY_S, VK_KEY_T, VK_KEY_U, VK_KEY_V, VK_KEY_W, VK_KEY_X, VK_KEY_Y, VK_KEY_Z, VK_RETURN, VK_SPACE,
    VK_RCONTROL, VK_RMENU, VK_RSHIFT, VK_LWIN, VK_OEM_PERIOD, VK_OEM_COMMA, VK_F1, VK_F2, VK_F3, VK_F4,
    VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12, VK_TAB, VK_DELETE,
    VK_OEM_1, VK_OEM_2, VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_MINUS, VK_OEM_PLUS, VK_DOWN,
    VK_LEFT, VK_RIGHT, VK_UP, VK_CAPITAL,
    _VK_KEY_MAX
};

struct KeyboardLocale {
    std::string name;
    std::string localization[_VK_KEY_MAX][2];
};

struct KeyboardState {
    bool keys[_VK_KEY_MAX];
};

class KeyboardView;

class ButtonView : public brls::Box {
  public:
    ButtonView(KeyboardView* keyboardView);
    ~ButtonView();
    brls::Label* charLabel;
    void setKey(KeyboardKeys key);
    void registerCallback();
    bool triggerType = false;
    KeyboardKeys key;
    std::function<void(void)> event = NULL;

    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style style, brls::FrameContext* ctx) override;
    void onFocusGained() override;
    void onFocusLost() override;

  private:
    brls::VoidEvent::Subscription shiftSubscription;
    bool dummy = true;
    bool eventPressed = false;
    bool focusJustGained = false;

    void applyTitle();
    KeyboardView* keyboardView;
};

class KeyboardView : public brls::Box {
  public:
    inline static brls::VoidEvent shiftUpdated;
    inline static std::vector<KeyboardLocale> getLocales() { return locales; }

    KeyboardView(bool focusable);
    ~KeyboardView();
    KeyboardState getKeyboardState();
    short getKeyCode(KeyboardKeys key);

    View* getParentNavigationDecision(View* from, View* newFocus,
                                      brls::FocusDirection direction) override;
    void draw(NVGcontext* vg, float x, float y, float width, float height,
              brls::Style style, brls::FrameContext* ctx) override;

  private:
    int keyboardLangLock = -1;
    bool needFocus = false;
    void createEnglishLayout();
    void createNumpadLayout();
    void createFullLayout();
    void changeLang(int lang);
    void createLocales();
    inline static std::vector<KeyboardLocale> locales;

    friend class ButtonView;
};
