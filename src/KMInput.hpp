#pragma once

#include <Windows.h>
#include <set>
#include <map>

/**
 * Class KMInput
 * Proceses Windows WM_INPUT messages via Raw Input API
 * Based on: http://molecularmusings.wordpress.com/2011/09/05/properly-handling-keyboard-input/
 * Supports only english layout (sufficient for games)
 * Known Bugs:
 */

//TODO - zrusit pre Linux tie hodnoty
enum Keys
{
    BACKSPACE = VK_BACK,
    TAB = VK_TAB,
    CLEAR = VK_CLEAR,
    ENTER = VK_RETURN,
    PAUSE = VK_PAUSE,
    CAPS_LOCK = VK_CAPITAL,
    ESCAPE = VK_ESCAPE,
    SPACE = VK_SPACE,
    PG_UP = VK_PRIOR,
    PG_DOWN = VK_NEXT,
    END = VK_END,
    HOME = VK_HOME,
    LEFT = VK_LEFT,
    UP = VK_UP,
    RIGHT = VK_RIGHT,
    DOWN = VK_DOWN,
    SELECT = VK_SELECT,
    PRINT = VK_PRINT,
    PRINT_SCREEN = VK_SNAPSHOT,
    INSERT = VK_INSERT,
    DEL = VK_DELETE,
    NUMBER_0 = '0',
    NUMBER_1 = '1',
    NUMBER_2 = '2',
    NUMBER_3 = '3',
    NUMBER_4 = '4',
    NUMBER_5 = '5',
    NUMBER_6 = '6',
    NUMBER_7 = '7',
    NUMBER_8 = '8',
    NUMBER_9 = '9',
    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',
    WIN = VK_LWIN,
    APPS = VK_APPS,
    NUMPAD_0 = VK_NUMPAD0,
    NUMPAD_1 = VK_NUMPAD1,
    NUMPAD_2 = VK_NUMPAD2,
    NUMPAD_3 = VK_NUMPAD3,
    NUMPAD_4 = VK_NUMPAD4,
    NUMPAD_5 = VK_NUMPAD5,
    NUMPAD_6 = VK_NUMPAD6,
    NUMPAD_7 = VK_NUMPAD7,
    NUMPAD_8 = VK_NUMPAD8,
    NUMPAD_9 = VK_NUMPAD9,
    NUMPAD_MULTIPLY = VK_MULTIPLY,
    NUMPAD_ADD = VK_ADD,
    NUMPAD_SEPARATOR = VK_SEPARATOR,
    NUMPAD_MINUS = VK_SUBTRACT,
    NUMPAD_DECIMAL = VK_DECIMAL,
    NUMPAD_DIVIDE = VK_DIVIDE,
    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,
    NUM_LOCK = VK_NUMLOCK,
    SCROLL_LOCK = VK_SCROLL,
    SHIFT_LEFT = VK_LSHIFT,
    SHIFT_RIGHT = VK_RSHIFT,
    CONTROL_LEFT = VK_LCONTROL,
    CONTROL_RIGHT = VK_RCONTROL,
    ALT_LEFT = VK_LMENU,
    ALT_RIGHT = VK_RMENU,
    SEMICOLON = VK_OEM_1,
    PLUS = VK_OEM_PLUS,
    COMMA = VK_OEM_COMMA,
    MINUS = VK_OEM_MINUS,
    PERIOD = VK_OEM_PERIOD,
    SLASH = VK_OEM_2,
    TILDE = VK_OEM_3,
    BRACE_LEFT = VK_OEM_4,
    BACKSLASH = VK_OEM_5,
    BRACE_RIGHT = VK_OEM_6,
    QUOTE = VK_OEM_7,
    NUMPAD_ENTER,
    UNKNOWN
};

std::string mapKeyToString(enum Keys key);

class KMInput
{
public:

    enum KMInputMode
    {
        KMIMPUT_KEYBOARD_MODE_SINGLE_DOWN, //when key is held down, produce only one keydown message
        KMINPUT_KEYBOARD_MODE_TYPEMATIC    //default behaviour - sends keydown message, even though the key is downpressed
    };

    enum KMInputMessageType
    {
        KMINPUT_MESSAGE_KEYBOARD,
        KMINPUT_MESSAGE_MOUSE
    };

    enum KMInputKeyState
    {
        KMINPUT_KEY_RELEASED = 0,
        KMINPUT_KEY_PRESSED = 1
    };

    enum KMInputMouseButtonState
    {
        KMINPUT_MOUSE_BUTTON_DOWN = 0,
        KMINPUT_MOUSE_BUTTON_UP
    };

    struct KMInputKeyboardMsg
    {
        enum Keys key;
        UINT virtualKeyCode; //If key is Key::UNKNOWN, let user handle this case
        enum KMInputKeyState state;
    };

    struct KMInputMouseMsg
    {
        int deltaX;
        int deltaY;
        int deltaWheel;

        KMInputMouseButtonState left;
        KMInputMouseButtonState middle;
        KMInputMouseButtonState right;
    };

    struct KMInputMessage
    {
        enum KMInputMessageType messageType;
        union
        {
            KMInputKeyboardMsg keyboard;
            KMInputMouseMsg    mouse;
        };
    };

	KMInput(HWND hwnd);

    //Returns singleton instance
    //static KMInput * getInstance();

    //Core function
    //Receives lparam from WM_INPUT to process
    //Remaps and adjusts scan codes to virtual keys
    const KMInputMessage* processInputMessage(LPARAM lParam);
    
    void setKeyboardMode(enum KMInputMode mode);
    enum KMInputMode getMode();

    void resetMouseButtons();

	bool isValid() { return _valid; }

private:
    /*static*/ HWND                         _h_WND; //Zamenit za Handle
    /*static*/ bool                         _valid;
    /*static*/ KMInputMessage               _msg;
    /*static*/ enum KMInputMode             _mode;
    /*static*/ std::set<enum Keys>          _keySet;       //initialized with keys except for UNKNOWN
    /*static*/ std::map<enum Keys, bool>    _keyPressedMap;//used when in Single Down mode
    
    /*static*/ KMInputMouseButtonState      _mouseButtonState[3]; //keeps state of 3 mouse buttons
};