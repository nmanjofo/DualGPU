#include "KMInput.hpp"

/*
bool KMInput::_valid = false;
enum KMInput::KMInputMode              KMInput::_mode = KMInput::KMInputMode::KMINPUT_KEYBOARD_MODE_TYPEMATIC;
KMInput::KMInputMessage                KMInput::_msg;
std::set<enum Keys>                    KMInput::_keySet;
std::map<enum Keys, bool>              KMInput::_keyPressedMap;
enum KMInput::KMInputMouseButtonState  KMInput::_mouseButtonState[3];
HWND                                   KMInput::_h_WND;
*/
std::string mapKeyToString(enum Keys key)
{
    switch (key)
    {
    case Keys::BACKSPACE:       return std::string("Backspace");
    case Keys::TAB:             return std::string("Tab");
    case Keys::CLEAR:           return std::string("Clear");
    case Keys::ENTER:           return std::string("Enter");
    case Keys::PAUSE:           return std::string("Pause");
    case Keys::CAPS_LOCK:       return std::string("CapsLock");
    case Keys::ESCAPE:          return std::string("Escape");
    case Keys::SPACE:           return std::string("Space");
    case Keys::PG_UP:           return std::string("Page Up");
    case Keys::PG_DOWN:         return std::string("Page Down");
    case Keys::END:             return std::string("End");
    case Keys::HOME:            return std::string("Home");
    case Keys::LEFT:            return std::string("Left");
    case Keys::UP:              return std::string("Up");
    case Keys::RIGHT:           return std::string("Right");
    case Keys::DOWN:            return std::string("Down");
    case Keys::SELECT:          return std::string("Select");
    case Keys::PRINT:           return std::string("Print");
    case Keys::PRINT_SCREEN:    return std::string("Print Screen");
    case Keys::INSERT:          return std::string("Insert");
    case Keys::DEL:             return std::string("Delete");
    case Keys::NUMBER_0:        return std::string("0");
    case Keys::NUMBER_1:        return std::string("1");
    case Keys::NUMBER_2:        return std::string("2");
    case Keys::NUMBER_3:        return std::string("3");
    case Keys::NUMBER_4:        return std::string("4");
    case Keys::NUMBER_5:        return std::string("5");
    case Keys::NUMBER_6:        return std::string("6");
    case Keys::NUMBER_7:        return std::string("7");
    case Keys::NUMBER_8:        return std::string("8");
    case Keys::NUMBER_9:        return std::string("9");
    case Keys::A:               return std::string("A");
    case Keys::B:               return std::string("B");
    case Keys::C:               return std::string("C");
    case Keys::D:               return std::string("D");
    case Keys::E:               return std::string("E");
    case Keys::F:               return std::string("F");
    case Keys::G:               return std::string("G");
    case Keys::H:               return std::string("H");
    case Keys::I:               return std::string("I");
    case Keys::J:               return std::string("J");
    case Keys::K:               return std::string("K");
    case Keys::L:               return std::string("L");
    case Keys::M:               return std::string("M");
    case Keys::N:               return std::string("N");
    case Keys::O:               return std::string("O");
    case Keys::P:               return std::string("P");
    case Keys::Q:               return std::string("Q");
    case Keys::R:               return std::string("R");
    case Keys::S:               return std::string("S");
    case Keys::T:               return std::string("T");
    case Keys::U:               return std::string("U");
    case Keys::V:               return std::string("V");
    case Keys::W:               return std::string("W");
    case Keys::X:               return std::string("X");
    case Keys::Y:               return std::string("Y");
    case Keys::Z:               return std::string("Z");
    case Keys::WIN:             return std::string("WIN Key");
    case Keys::APPS:            return std::string("Apps");
    case Keys::NUMPAD_0:        return std::string("NUMPAD 0");
    case Keys::NUMPAD_1:        return std::string("NUMPAD 1");
    case Keys::NUMPAD_2:        return std::string("NUMPAD 2");
    case Keys::NUMPAD_3:        return std::string("NUMPAD 3");
    case Keys::NUMPAD_4:        return std::string("NUMPAD 4");
    case Keys::NUMPAD_5:        return std::string("NUMPAD 5");
    case Keys::NUMPAD_6:        return std::string("NUMPAD 6");
    case Keys::NUMPAD_7:        return std::string("NUMPAD 7");
    case Keys::NUMPAD_8:        return std::string("NUMPAD 8");
    case Keys::NUMPAD_9:        return std::string("NUMPAD 9");
    case Keys::NUMPAD_MULTIPLY: return std::string("NUMPAD *");
    case Keys::NUMPAD_ADD:      return std::string("NUMPAD +");
    case Keys::NUMPAD_SEPARATOR:return std::string("NUMPAD \u2396");
    case Keys::NUMPAD_MINUS:    return std::string("NUMPAD -");
    case Keys::NUMPAD_DECIMAL:  return std::string("NUMPAD .");
    case Keys::NUMPAD_DIVIDE:   return std::string("NUMPAD /");
    case Keys::F1:              return std::string("F1");
    case Keys::F2:              return std::string("F2");
    case Keys::F3:              return std::string("F3");
    case Keys::F4:              return std::string("F4");
    case Keys::F5:              return std::string("F5");
    case Keys::F6:              return std::string("F6");
    case Keys::F7:              return std::string("F7");
    case Keys::F8:              return std::string("F8");
    case Keys::F9:              return std::string("F9");
    case Keys::F10:             return std::string("F10");
    case Keys::F11:             return std::string("F11");
    case Keys::F12:             return std::string("F12");
    case Keys::NUM_LOCK:        return std::string("Num Lock");
    case Keys::SCROLL_LOCK:     return std::string("Scroll Lock");
    case Keys::SHIFT_LEFT:      return std::string("Left Shift");
    case Keys::SHIFT_RIGHT:     return std::string("Right Shift");
    case Keys::CONTROL_LEFT:    return std::string("Left Control");
    case Keys::CONTROL_RIGHT:   return std::string("Right Control");
    case Keys::ALT_LEFT:        return std::string("Left Alt");
    case Keys::ALT_RIGHT:       return std::string("Right Alt");
    case Keys::SEMICOLON:       return std::string(";");
    case Keys::PLUS:            return std::string("+");
    case Keys::COMMA:           return std::string(",");
    case Keys::MINUS:           return std::string("-");
    case Keys::PERIOD:          return std::string(".");
    case Keys::SLASH:           return std::string("/");
    case Keys::TILDE:           return std::string("~");
    case Keys::BRACE_LEFT:      return std::string("[");
    case Keys::BACKSLASH:       return std::string("\\");
    case Keys::BRACE_RIGHT:     return std::string("]");
    case Keys::QUOTE:           return std::string("'");
    case Keys::NUMPAD_ENTER:    return std::string("NUMPAD Enter");
    default:                    return std::string("");
    }
}

const KMInput::KMInputMessage* KMInput::processInputMessage(LPARAM lParam)
{
    if (!_valid)
        return NULL;

    //Get raw input data
    char buffer[sizeof(RAWINPUT)] = {};
    UINT size = sizeof(RAWINPUT);
    GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

    //Extract raw input data
    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer);

    /* Process keyboard data */
    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        const RAWKEYBOARD& rawKB = raw->data.keyboard;
        
        //Character?
        UINT virtualKey = rawKB.VKey;
        UINT scanCode = rawKB.MakeCode;
        UINT flags = rawKB.Flags;

        //Correcting scan codes/virtual keys:
        if (virtualKey == 255)
        {
            // discard "fake keys" which are part of an escaped sequence
            return NULL;
        }
        else if (virtualKey == VK_SHIFT)
        {
            // correct left-hand / right-hand SHIFT
            virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
        }
        else if (virtualKey == VK_NUMLOCK)
        {
            // correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
            scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
        }

        // e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
        // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
        const bool isE0 = ((flags & RI_KEY_E0) != 0);
        const bool isE1 = ((flags & RI_KEY_E1) != 0);

        if (isE1)
        {
            // for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
            // however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
            if (virtualKey == VK_PAUSE)
                scanCode = 0x45;
            else
                scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
        }

        enum Keys key = (enum Keys)virtualKey;

        switch (virtualKey)
        {
            // right-hand CONTROL and ALT have their e0 bit set
        case VK_CONTROL:
            if (isE0)
            {
                virtualKey = VK_RCONTROL;
                key = (enum Keys) VK_RCONTROL;
            }
            else
            {
                virtualKey = VK_LCONTROL;
                key = (enum Keys)VK_LCONTROL;
            }
            break;

        case VK_MENU:
            if (isE0)
            {
                virtualKey = VK_RMENU;
                key = (enum Keys)VK_RMENU;
            }
            else
            {
                virtualKey = VK_LMENU;
                key = (enum Keys)VK_LMENU;
            }
            break;

            // NUMPAD ENTER has its e0 bit set
        case VK_RETURN:
            if (isE0)
               key = Keys::NUMPAD_ENTER;
            break;

            // the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys will always have their e0 bit set, but the
            // corresponding keys on the NUMPAD will not.
        case VK_INSERT:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD0;
                key = (enum Keys) VK_NUMPAD0;
            }
            break;

        case VK_DELETE:
            if (!isE0)
            {
                virtualKey = VK_DECIMAL;
                key = (enum Keys)VK_DECIMAL;
            }
            break;

        case VK_HOME:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD7;
                key = (enum Keys) VK_NUMPAD7;
            }
            break;

        case VK_END:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD1;
                key = (enum Keys)VK_NUMPAD1;
            }
            break;

        case VK_PRIOR:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD9;
                key = (enum Keys)VK_NUMPAD9;
            }
            break;

        case VK_NEXT:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD3;
                key = (enum Keys)VK_NUMPAD3;
            }
            break;

            // the standard arrow keys will always have their e0 bit set, but the
            // corresponding keys on the NUMPAD will not.
        case VK_LEFT:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD4;
                key = (enum Keys)VK_NUMPAD4;
            }
            break;

        case VK_RIGHT:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD6;
                key = (enum Keys)VK_NUMPAD6;
            }
            break;

        case VK_UP:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD8;
                key = (enum Keys)VK_NUMPAD8;
            }
            break;

        case VK_DOWN:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD2;
                key = (enum Keys)VK_NUMPAD2;
            }
            break;

            // NUMPAD 5 doesn't have its e0 bit set
        case VK_CLEAR:
            if (!isE0)
            {
                virtualKey = VK_NUMPAD5;
                key = (enum Keys)VK_NUMPAD5;
            }
            break;
        
        default:
            break;
        }

        //If in Single Down mode, return NULL if key didn't change it's state since last time
        if (_mode == KMIMPUT_KEYBOARD_MODE_SINGLE_DOWN)
        {
            if (_keyPressedMap[key]==true && (rawKB.Flags & RI_KEY_BREAK) == 0)
                return NULL;
        }

        if (_keySet.find((enum Keys)virtualKey) != _keySet.end())
        {
            _msg.messageType = KMINPUT_MESSAGE_KEYBOARD;
            _msg.keyboard.key = key;
            _msg.keyboard.virtualKeyCode = virtualKey;
            _msg.keyboard.state = (rawKB.Flags & RI_KEY_BREAK)==0 ? KMINPUT_KEY_PRESSED : KMINPUT_KEY_RELEASED;

            if (_mode==KMIMPUT_KEYBOARD_MODE_SINGLE_DOWN)
            {
                _keyPressedMap[key] = (rawKB.Flags & RI_KEY_BREAK) == 0;
            }
        }
        else
        {
            _msg.messageType = KMINPUT_MESSAGE_KEYBOARD;
            _msg.keyboard.key = Keys::UNKNOWN;
            _msg.keyboard.virtualKeyCode = virtualKey;
            _msg.keyboard.state = (rawKB.Flags & RI_KEY_BREAK) == 0 ? KMINPUT_KEY_PRESSED : KMINPUT_KEY_RELEASED;
        }

        return &_msg;
    }
    /* Process mouse data */
    else if (raw->header.dwType == RIM_TYPEMOUSE)
    {
        _msg.messageType = KMInputMessageType::KMINPUT_MESSAGE_MOUSE;
        _msg.mouse.deltaWheel = raw->data.mouse.usButtonFlags == RI_MOUSE_WHEEL ? (short)raw->data.mouse.usButtonData : 0;

        bool left_changed =   bool(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)   ^ bool(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP);
        bool right_changed =  bool(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)  ^ bool(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP);
        bool middle_changed = bool(raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) ^ bool(raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP);

        if (left_changed)
        {
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
            {
                _mouseButtonState[0] = KMINPUT_MOUSE_BUTTON_DOWN;
            }
            else
            {
                _mouseButtonState[0] = KMINPUT_MOUSE_BUTTON_UP;
            }
        }

        if (middle_changed)
        {
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
            {
                _mouseButtonState[1] = KMINPUT_MOUSE_BUTTON_DOWN;
            }
            else
            {
                _mouseButtonState[1] = KMINPUT_MOUSE_BUTTON_UP;
            }
        }

        if (right_changed)
        {
            if (raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
            {
                _mouseButtonState[2] = KMINPUT_MOUSE_BUTTON_DOWN;
            }
            else
            {
                _mouseButtonState[2] = KMINPUT_MOUSE_BUTTON_UP;
            }
        }

        _msg.mouse.left   = _mouseButtonState[0];
        _msg.mouse.middle = _mouseButtonState[1];
        _msg.mouse.right  = _mouseButtonState[2];

        _msg.mouse.deltaX = raw->data.mouse.lLastX;
        _msg.mouse.deltaY = raw->data.mouse.lLastY;

        return &_msg;
    }
    else
        return NULL;
}

KMInput::KMInput(HWND _h_WND)
{
    /*
	//Get window handle
    HWND tmp;
    _h_WND = GetActiveWindow();
    
    while ((tmp = GetParent(_h_WND)) != NULL)
        _h_WND = tmp;
	*/

	//--ZHORA init
	_valid = false;
	_mode = KMInput::KMInputMode::KMINPUT_KEYBOARD_MODE_TYPEMATIC;
	//--

    //Register raw input
    RAWINPUTDEVICE inputDevices[2];

    //Mouse
    inputDevices[0].usUsagePage = 0x01;
    inputDevices[0].usUsage = 0x02;
    inputDevices[0].dwFlags = 0;
    inputDevices[0].hwndTarget = _h_WND;

    //Keyboard
    inputDevices[1].usUsagePage = 0x01;
    inputDevices[1].usUsage = 0x06;
    inputDevices[1].dwFlags = 0;
    inputDevices[1].hwndTarget = _h_WND;

    if (RegisterRawInputDevices(inputDevices, 2, sizeof(inputDevices[0])) == FALSE)
    {
        _valid = false;
		return;
    }

    //Initialize key set, push all values except for UNKNOWN
    _keySet.insert(Keys::BACKSPACE);
    _keySet.insert(Keys::TAB);
    _keySet.insert(Keys::CLEAR);
    _keySet.insert(Keys::ENTER);
    _keySet.insert(Keys::PAUSE);
    _keySet.insert(Keys::CAPS_LOCK);
    _keySet.insert(Keys::ESCAPE);
    _keySet.insert(Keys::SPACE);
    _keySet.insert(Keys::PG_UP);
    _keySet.insert(Keys::PG_DOWN);
    _keySet.insert(Keys::END);
    _keySet.insert(Keys::HOME);
    _keySet.insert(Keys::LEFT);
    _keySet.insert(Keys::UP);
    _keySet.insert(Keys::RIGHT);
    _keySet.insert(Keys::DOWN);
    _keySet.insert(Keys::SELECT);
    _keySet.insert(Keys::PRINT);
    _keySet.insert(Keys::PRINT_SCREEN);
    _keySet.insert(Keys::INSERT);
    _keySet.insert(Keys::DEL);
    _keySet.insert(Keys::NUMBER_0);
    _keySet.insert(Keys::NUMBER_1);
    _keySet.insert(Keys::NUMBER_2);
    _keySet.insert(Keys::NUMBER_3);
    _keySet.insert(Keys::NUMBER_4);
    _keySet.insert(Keys::NUMBER_5);
    _keySet.insert(Keys::NUMBER_6);
    _keySet.insert(Keys::NUMBER_7);
    _keySet.insert(Keys::NUMBER_8);
    _keySet.insert(Keys::NUMBER_9);
    _keySet.insert(Keys::A);
    _keySet.insert(Keys::B);
    _keySet.insert(Keys::C);
    _keySet.insert(Keys::D);
    _keySet.insert(Keys::E);
    _keySet.insert(Keys::F);
    _keySet.insert(Keys::G);
    _keySet.insert(Keys::H);
    _keySet.insert(Keys::I);
    _keySet.insert(Keys::J);
    _keySet.insert(Keys::K);
    _keySet.insert(Keys::L);
    _keySet.insert(Keys::M);
    _keySet.insert(Keys::N);
    _keySet.insert(Keys::O);
    _keySet.insert(Keys::P);
    _keySet.insert(Keys::Q);
    _keySet.insert(Keys::R);
    _keySet.insert(Keys::S);
    _keySet.insert(Keys::T);
    _keySet.insert(Keys::U);
    _keySet.insert(Keys::V);
    _keySet.insert(Keys::W);
    _keySet.insert(Keys::X);
    _keySet.insert(Keys::Y);
    _keySet.insert(Keys::Z);
    _keySet.insert(Keys::WIN);
    _keySet.insert(Keys::APPS);
    _keySet.insert(Keys::NUMPAD_0);
    _keySet.insert(Keys::NUMPAD_1);
    _keySet.insert(Keys::NUMPAD_2);
    _keySet.insert(Keys::NUMPAD_3);
    _keySet.insert(Keys::NUMPAD_4);
    _keySet.insert(Keys::NUMPAD_5);
    _keySet.insert(Keys::NUMPAD_6);
    _keySet.insert(Keys::NUMPAD_7);
    _keySet.insert(Keys::NUMPAD_8);
    _keySet.insert(Keys::NUMPAD_9);
    _keySet.insert(Keys::NUMPAD_MULTIPLY);
    _keySet.insert(Keys::NUMPAD_ADD);
    _keySet.insert(Keys::NUMPAD_SEPARATOR);
    _keySet.insert(Keys::NUMPAD_MINUS);
    _keySet.insert(Keys::NUMPAD_DECIMAL);
    _keySet.insert(Keys::NUMPAD_DIVIDE);
    _keySet.insert(Keys::F1);
    _keySet.insert(Keys::F2);
    _keySet.insert(Keys::F3);
    _keySet.insert(Keys::F4);
    _keySet.insert(Keys::F5);
    _keySet.insert(Keys::F6);
    _keySet.insert(Keys::F7);
    _keySet.insert(Keys::F8);
    _keySet.insert(Keys::F9);
    _keySet.insert(Keys::F10);
    _keySet.insert(Keys::F11);
    _keySet.insert(Keys::F12);
    _keySet.insert(Keys::NUM_LOCK);
    _keySet.insert(Keys::SCROLL_LOCK);
    _keySet.insert(Keys::SHIFT_LEFT);
    _keySet.insert(Keys::SHIFT_RIGHT);
    _keySet.insert(Keys::CONTROL_LEFT);
    _keySet.insert(Keys::CONTROL_RIGHT);
    _keySet.insert(Keys::ALT_LEFT);
    _keySet.insert(Keys::ALT_RIGHT);
    _keySet.insert(Keys::SEMICOLON);
    _keySet.insert(Keys::PLUS);
    _keySet.insert(Keys::COMMA);
    _keySet.insert(Keys::MINUS);
    _keySet.insert(Keys::PERIOD);
    _keySet.insert(Keys::SLASH);
    _keySet.insert(Keys::TILDE);
    _keySet.insert(Keys::BRACE_LEFT);
    _keySet.insert(Keys::BACKSLASH);
    _keySet.insert(Keys::BRACE_RIGHT);
    _keySet.insert(Keys::QUOTE);
    _keySet.insert(Keys::NUMPAD_ENTER);

    //Set mouse button states to all up
    _mouseButtonState[0] = KMINPUT_MOUSE_BUTTON_UP;
    _mouseButtonState[1] = KMINPUT_MOUSE_BUTTON_UP;
    _mouseButtonState[2] = KMINPUT_MOUSE_BUTTON_UP;

    _valid = true;
}

/*
KMInput* KMInput::getInstance()
{
    static KMInput instance;
    if (_valid)
        return &instance;
    else
        return NULL;
}*/

void KMInput::setKeyboardMode(enum KMInput::KMInputMode mode)
{
    _mode = mode;
}

enum KMInput::KMInputMode KMInput::getMode()
{
    return _mode;
}

void KMInput::resetMouseButtons()
{
    _mouseButtonState[0] = KMINPUT_MOUSE_BUTTON_UP;
    _mouseButtonState[1] = KMINPUT_MOUSE_BUTTON_UP;
    _mouseButtonState[2] = KMINPUT_MOUSE_BUTTON_UP;
}
